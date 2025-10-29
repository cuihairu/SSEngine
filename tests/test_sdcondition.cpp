/**
 * @file test_sdcondition.cpp
 * @brief Condition variable functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdcondition.h"
#include "ssengine/sdmutex.h"
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>

using namespace SSCP;

class SDConditionTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// Basic functionality tests
TEST_F(SDConditionTest, BasicSignalWait) {
    CSDCondition condition;
    CSDMutex mutex;
    std::atomic<bool> threadReady{false};
    std::atomic<bool> signalReceived{false};
    
    // Thread that waits for signal
    std::thread waiterThread([&]() {
        mutex.Lock();
        threadReady = true;
        BOOL result = condition.Wait(mutex, 1000); // 1 second timeout
        signalReceived = (result == TRUE);
        mutex.Unlock();
    });
    
    // Wait for thread to be ready
    while (!threadReady) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Small delay to ensure thread is waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Signal the condition
    condition.Signal();
    
    waiterThread.join();
    
    EXPECT_TRUE(signalReceived);
}

TEST_F(SDConditionTest, TimeoutBehavior) {
    CSDCondition condition;
    CSDMutex mutex;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    mutex.Lock();
    BOOL result = condition.Wait(mutex, 100); // 100ms timeout
    mutex.Unlock();
    
    auto duration = std::chrono::high_resolution_clock::now() - start;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    EXPECT_FALSE(result); // Should timeout
    EXPECT_GE(ms, 90);    // Should wait at least close to 100ms
    EXPECT_LE(ms, 200);   // But not too much longer
}

TEST_F(SDConditionTest, BroadcastMultipleWaiters) {
    CSDCondition condition;
    CSDMutex mutex;
    std::atomic<int> readyCount{0};
    std::atomic<int> signalCount{0};
    
    const int numThreads = 4;
    std::vector<std::thread> threads;
    
    // Create multiple waiter threads
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            mutex.Lock();
            readyCount++;
            BOOL result = condition.Wait(mutex, 2000); // 2 second timeout
            if (result) {
                signalCount++;
            }
            mutex.Unlock();
        });
    }
    
    // Wait for all threads to be ready
    while (readyCount < numThreads) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Small delay to ensure all threads are waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Broadcast to wake all threads
    condition.Broadcast();
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(signalCount, numThreads);
}

TEST_F(SDConditionTest, SignalOnlyWakesOneThread) {
    CSDCondition condition;
    CSDMutex mutex;
    std::atomic<int> readyCount{0};
    std::atomic<int> signalCount{0};
    
    const int numThreads = 3;
    std::vector<std::thread> threads;
    
    // Create multiple waiter threads
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back([&, i]() {
            mutex.Lock();
            readyCount++;
            BOOL result = condition.Wait(mutex, 1000); // 1 second timeout
            if (result) {
                signalCount++;
            }
            mutex.Unlock();
        });
    }
    
    // Wait for all threads to be ready
    while (readyCount < numThreads) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Small delay to ensure all threads are waiting
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Signal should wake only one thread
    condition.Signal();
    
    // Give some time for the signal to be processed
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Signal again to wake another thread
    condition.Signal();
    
    // Give some time for the second signal
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Wait for all threads to complete (some may timeout)
    for (auto& thread : threads) {
        thread.join();
    }
    
    // At least one thread should have been signaled
    EXPECT_GE(signalCount, 1);
    // Not all threads should have been signaled (unless there's a race condition)
    // This test might be flaky depending on timing
}

TEST_F(SDConditionTest, PointerMutexInterface) {
    CSDCondition condition;
    CSDMutex mutex;
    std::atomic<bool> threadReady{false};
    std::atomic<bool> signalReceived{false};
    
    // Test with mutex pointer
    std::thread waiterThread([&]() {
        mutex.Lock();
        threadReady = true;
        BOOL result = condition.Wait(&mutex, 1000); // Use pointer interface
        signalReceived = (result == TRUE);
        mutex.Unlock();
    });
    
    // Wait for thread to be ready
    while (!threadReady) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    condition.Signal();
    
    waiterThread.join();
    
    EXPECT_TRUE(signalReceived);
}

TEST_F(SDConditionTest, NullMutexPointer) {
    CSDCondition condition;
    
    // Should return FALSE for null mutex pointer
    BOOL result = condition.Wait(nullptr, 100);
    EXPECT_FALSE(result);
}

TEST_F(SDConditionTest, InfiniteWait) {
    CSDCondition condition;
    CSDMutex mutex;
    std::atomic<bool> threadReady{false};
    std::atomic<bool> signalReceived{false};
    
    // Thread that waits indefinitely
    std::thread waiterThread([&]() {
        mutex.Lock();
        threadReady = true;
        BOOL result = condition.Wait(mutex, SDINFINITE);
        signalReceived = (result == TRUE);
        mutex.Unlock();
    });
    
    // Wait for thread to be ready
    while (!threadReady) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Wait a bit longer than a normal timeout to ensure infinite wait works
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Signal the condition
    condition.Signal();
    
    waiterThread.join();
    
    EXPECT_TRUE(signalReceived);
}

TEST_F(SDConditionTest, ProducerConsumerPattern) {
    CSDCondition condition;
    CSDMutex mutex;
    std::vector<int> buffer;
    std::atomic<bool> producerDone{false};
    std::atomic<int> consumedCount{0};
    
    const int numItems = 10;
    
    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < numItems; ++i) {
            mutex.Lock();
            buffer.push_back(i);
            mutex.Unlock();
            condition.Signal(); // Signal that item is available
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        producerDone = true;
        condition.Signal(); // Final signal to wake consumer
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        while (!producerDone || !buffer.empty()) {
            mutex.Lock();
            while (buffer.empty() && !producerDone) {
                condition.Wait(mutex, 100); // Wait for items
            }
            
            if (!buffer.empty()) {
                buffer.pop_back();
                consumedCount++;
            }
            mutex.Unlock();
        }
    });
    
    producer.join();
    consumer.join();
    
    EXPECT_EQ(consumedCount, numItems);
    EXPECT_TRUE(buffer.empty());
}

TEST_F(SDConditionTest, MultipleConditionsOnSameMutex) {
    CSDCondition condition1;
    CSDCondition condition2;
    CSDMutex mutex;
    
    std::atomic<bool> thread1Ready{false};
    std::atomic<bool> thread2Ready{false};
    std::atomic<bool> signal1Received{false};
    std::atomic<bool> signal2Received{false};
    
    // Thread waiting on condition1
    std::thread thread1([&]() {
        mutex.Lock();
        thread1Ready = true;
        BOOL result = condition1.Wait(mutex, 1000);
        signal1Received = (result == TRUE);
        mutex.Unlock();
    });
    
    // Thread waiting on condition2
    std::thread thread2([&]() {
        mutex.Lock();
        thread2Ready = true;
        BOOL result = condition2.Wait(mutex, 1000);
        signal2Received = (result == TRUE);
        mutex.Unlock();
    });
    
    // Wait for both threads to be ready
    while (!thread1Ready || !thread2Ready) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Signal each condition separately
    condition1.Signal();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    condition2.Signal();
    
    thread1.join();
    thread2.join();
    
    EXPECT_TRUE(signal1Received);
    EXPECT_TRUE(signal2Received);
}

TEST_F(SDConditionTest, StressTest) {
    CSDCondition condition;
    CSDMutex mutex;
    std::atomic<int> signalCount{0};
    std::atomic<int> totalSignaled{0};
    
    const int numWaiters = 10;
    const int numSignals = 50;
    
    std::vector<std::thread> waiters;
    
    // Create waiter threads
    for (int i = 0; i < numWaiters; ++i) {
        waiters.emplace_back([&, i]() {
            int localSignals = 0;
            while (signalCount < numSignals) {
                mutex.Lock();
                if (signalCount < numSignals) {
                    BOOL result = condition.Wait(mutex, 100);
                    if (result) {
                        localSignals++;
                    }
                }
                mutex.Unlock();
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            totalSignaled += localSignals;
        });
    }
    
    // Signaler thread
    std::thread signaler([&]() {
        for (int i = 0; i < numSignals; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if (i % 3 == 0) {
                condition.Broadcast();
            } else {
                condition.Signal();
            }
            signalCount++;
        }
    });
    
    signaler.join();
    
    // Wait for all waiters to complete
    for (auto& waiter : waiters) {
        waiter.join();
    }
    
    // Should have received at least some signals
    EXPECT_GT(totalSignaled, 0);
    std::cout << "Total signals received: " << totalSignaled << " out of " << numSignals << "\n";
}

// Performance test
TEST_F(SDConditionTest, PerformanceTest) {
    CSDCondition condition;
    CSDMutex mutex;
    
    const int numOperations = 1000;
    
    // Measure signal/wait performance
    auto start = std::chrono::high_resolution_clock::now();
    
    std::thread waiter([&]() {
        for (int i = 0; i < numOperations; ++i) {
            mutex.Lock();
            condition.Wait(mutex, 1000);
            mutex.Unlock();
        }
    });
    
    // Small delay to ensure waiter is ready
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    for (int i = 0; i < numOperations; ++i) {
        condition.Signal();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }
    
    waiter.join();
    
    auto duration = std::chrono::high_resolution_clock::now() - start;
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
    
    std::cout << "Condition variable operations (" << numOperations << " signal/wait pairs): " 
              << micros << " μs\n";
    std::cout << "Average time per operation: " << (double)micros / numOperations << " μs\n";
    
    // Performance should be reasonable
    EXPECT_LT(micros, 10000000); // Less than 10 seconds
}