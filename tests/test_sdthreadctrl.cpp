/**
 * @file test_sdthreadctrl.cpp
 * @brief Thread control functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdthreadctrl.h"
#include <atomic>
#include <chrono>
#include <vector>
#include <thread>

using namespace SSCP;

class SDThreadCtrlTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// Test thread implementations
class TestThread : public ISSThread {
public:
    TestThread() : running(true), executed(false), terminateCalled(false) {}
    
    void SSAPI ThrdProc() override {
        executed = true;
        while (running) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    
    void SSAPI Terminate() override {
        terminateCalled = true;
        running = false;
    }
    
    std::atomic<bool> running{true};
    std::atomic<bool> executed{false};
    std::atomic<bool> terminateCalled{false};
};

class QuickThread : public ISSThread {
public:
    QuickThread() : executed(false) {}
    
    void SSAPI ThrdProc() override {
        executed = true;
        // Quick execution
    }
    
    void SSAPI Terminate() override {
        // Nothing special to do
    }
    
    std::atomic<bool> executed{false};
};

class CountingThread : public ISSThread {
public:
    CountingThread(int maxCount) : maxCount(maxCount), counter(0), shouldStop(false) {}
    
    void SSAPI ThrdProc() override {
        while (!shouldStop && counter < maxCount) {
            counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    void SSAPI Terminate() override {
        shouldStop = true;
    }
    
    int maxCount;
    std::atomic<int> counter{0};
    std::atomic<bool> shouldStop{false};
};

// Basic functionality tests
TEST_F(SDThreadCtrlTest, CreateAndBasicOperations) {
    TestThread* thread = new TestThread();
    ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
    
    EXPECT_NE(ctrl, nullptr);
    EXPECT_EQ(ctrl->GetThread(), thread);
    EXPECT_NE(ctrl->GetThreadId(), 0);
    
    // Wait a bit for thread to start
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(thread->executed);
    
    // Terminate and wait
    ctrl->Terminate();
    EXPECT_TRUE(ctrl->WaitFor(1000));
    EXPECT_TRUE(thread->terminateCalled);
    
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, QuickTerminatingThread) {
    QuickThread* thread = new QuickThread();
    ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
    
    EXPECT_NE(ctrl, nullptr);
    
    // Wait for quick thread to complete
    EXPECT_TRUE(ctrl->WaitFor(1000));
    EXPECT_TRUE(thread->executed);
    
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, ThreadIdUniqueness) {
    const int numThreads = 5;
    std::vector<ISSThreadCtrl*> controllers;
    std::vector<UINT32> threadIds;
    
    // Create multiple threads
    for (int i = 0; i < numThreads; ++i) {
        TestThread* thread = new TestThread();
        ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
        controllers.push_back(ctrl);
        threadIds.push_back(ctrl->GetThreadId());
    }
    
    // Verify all thread IDs are unique
    for (int i = 0; i < numThreads; ++i) {
        for (int j = i + 1; j < numThreads; ++j) {
            EXPECT_NE(threadIds[i], threadIds[j]);
        }
    }
    
    // Cleanup
    for (auto* ctrl : controllers) {
        ctrl->Terminate();
        ctrl->WaitFor(1000);
        ctrl->Release();
    }
}

TEST_F(SDThreadCtrlTest, WaitForTimeout) {
    TestThread* thread = new TestThread();
    ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
    
    // Wait with short timeout - should timeout since thread is running
    auto start = std::chrono::high_resolution_clock::now();
    bool result = ctrl->WaitFor(100); // 100ms timeout
    auto duration = std::chrono::high_resolution_clock::now() - start;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    EXPECT_FALSE(result); // Should timeout
    EXPECT_GE(ms, 90);    // Should wait approximately 100ms
    EXPECT_LE(ms, 200);   // But not too much longer
    
    // Now terminate and wait - should succeed
    ctrl->Terminate();
    EXPECT_TRUE(ctrl->WaitFor(1000));
    
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, WaitForInfinite) {
    CountingThread* thread = new CountingThread(10);
    ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
    
    // Wait indefinitely for thread to complete naturally
    auto start = std::chrono::high_resolution_clock::now();
    EXPECT_TRUE(ctrl->WaitFor(SD_INFINITE));
    auto duration = std::chrono::high_resolution_clock::now() - start;
    
    EXPECT_GE(thread->counter, 10);
    
    // Should have taken some time but not too long
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    EXPECT_GT(ms, 5);      // At least some time
    EXPECT_LT(ms, 1000);   // But not too long
    
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, KillMethod) {
    TestThread* thread = new TestThread();
    ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
    
    // Wait for thread to start
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(thread->executed);
    
    // Kill thread
    EXPECT_TRUE(ctrl->Kill(1));
    EXPECT_TRUE(thread->terminateCalled);
    
    // Should be able to wait after kill
    EXPECT_TRUE(ctrl->WaitFor(1000));
    
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, ResumeAndSuspendNoOp) {
    TestThread* thread = new TestThread();
    ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
    
    // These methods are no-op but should not crash
    ctrl->Suspend();
    ctrl->Resume();
    
    // Thread should still be running normally
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    EXPECT_TRUE(thread->executed);
    
    ctrl->Terminate();
    ctrl->WaitFor(1000);
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, MultipleTerminateCalls) {
    TestThread* thread = new TestThread();
    ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
    
    // Wait for thread to start
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Call terminate multiple times - should be safe
    ctrl->Terminate();
    ctrl->Terminate();
    ctrl->Terminate();
    
    EXPECT_TRUE(thread->terminateCalled);
    EXPECT_TRUE(ctrl->WaitFor(1000));
    
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, GetThreadMethod) {
    TestThread* originalThread = new TestThread();
    ISSThreadCtrl* ctrl = CreateThreadCtrl(originalThread);
    
    // GetThread should return the same thread instance
    ISSThread* retrievedThread = ctrl->GetThread();
    EXPECT_EQ(retrievedThread, originalThread);
    
    // Should be able to cast back and verify
    TestThread* castThread = static_cast<TestThread*>(retrievedThread);
    EXPECT_EQ(castThread, originalThread);
    
    ctrl->Terminate();
    ctrl->WaitFor(1000);
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, NullThreadHandling) {
    // Test with null thread - should handle gracefully
    ISSThreadCtrl* ctrl = CreateThreadCtrl(nullptr);
    
    EXPECT_NE(ctrl, nullptr);
    EXPECT_EQ(ctrl->GetThread(), nullptr);
    
    // Operations should not crash
    ctrl->Terminate();
    ctrl->Resume();
    ctrl->Suspend();
    EXPECT_TRUE(ctrl->Kill(0));
    EXPECT_TRUE(ctrl->WaitFor(100));
    
    ctrl->Release();
}

TEST_F(SDThreadCtrlTest, ThreadLifecycleManagement) {
    const int numIterations = 10;
    
    for (int i = 0; i < numIterations; ++i) {
        CountingThread* thread = new CountingThread(5);
        ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
        
        // Let thread run a bit
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // Some threads we let finish naturally, others we terminate
        if (i % 2 == 0) {
            ctrl->Terminate();
        }
        
        EXPECT_TRUE(ctrl->WaitFor(1000));
        ctrl->Release();
    }
}

TEST_F(SDThreadCtrlTest, ConcurrentThreadControl) {
    const int numThreads = 10;
    std::vector<ISSThreadCtrl*> controllers;
    std::vector<std::thread> controlThreads;
    
    // Create multiple worker threads
    for (int i = 0; i < numThreads; ++i) {
        CountingThread* thread = new CountingThread(100);
        ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
        controllers.push_back(ctrl);
    }
    
    // Create control threads to manage worker threads
    std::atomic<int> completedCount{0};
    
    for (int i = 0; i < numThreads; ++i) {
        controlThreads.emplace_back([&, i]() {
            auto* ctrl = controllers[i];
            
            // Let thread run for a bit
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            
            // Terminate and wait
            ctrl->Terminate();
            if (ctrl->WaitFor(1000)) {
                completedCount++;
            }
        });
    }
    
    // Wait for all control threads
    for (auto& t : controlThreads) {
        t.join();
    }
    
    EXPECT_EQ(completedCount, numThreads);
    
    // Cleanup
    for (auto* ctrl : controllers) {
        ctrl->Release();
    }
}

// Performance test
TEST_F(SDThreadCtrlTest, PerformanceTest) {
    const int numThreads = 50;
    std::vector<ISSThreadCtrl*> controllers;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create many threads
    for (int i = 0; i < numThreads; ++i) {
        QuickThread* thread = new QuickThread();
        ISSThreadCtrl* ctrl = CreateThreadCtrl(thread);
        controllers.push_back(ctrl);
    }
    
    auto createTime = std::chrono::high_resolution_clock::now() - start;
    
    // Wait for all threads to complete
    start = std::chrono::high_resolution_clock::now();
    
    for (auto* ctrl : controllers) {
        EXPECT_TRUE(ctrl->WaitFor(1000));
    }
    
    auto waitTime = std::chrono::high_resolution_clock::now() - start;
    
    // Cleanup
    start = std::chrono::high_resolution_clock::now();
    
    for (auto* ctrl : controllers) {
        ctrl->Release();
    }
    
    auto releaseTime = std::chrono::high_resolution_clock::now() - start;
    
    auto createMicros = std::chrono::duration_cast<std::chrono::microseconds>(createTime).count();
    auto waitMicros = std::chrono::duration_cast<std::chrono::microseconds>(waitTime).count();
    auto releaseMicros = std::chrono::duration_cast<std::chrono::microseconds>(releaseTime).count();
    
    std::cout << "Thread control performance for " << numThreads << " threads:\n";
    std::cout << "Creation time: " << createMicros << " μs\n";
    std::cout << "Wait time: " << waitMicros << " μs\n";
    std::cout << "Release time: " << releaseMicros << " μs\n";
    
    // Performance should be reasonable
    EXPECT_LT(createMicros, 10000000);  // Less than 10 seconds
    EXPECT_LT(waitMicros, 5000000);     // Less than 5 seconds
    EXPECT_LT(releaseMicros, 1000000);  // Less than 1 second
}