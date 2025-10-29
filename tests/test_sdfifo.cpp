/**
 * @file test_sdfifo.cpp
 * @brief FIFO queue functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdfifo.h"
#include "ssengine/sdmutex.h"
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

using namespace SSCP;

class SDFifoTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// Test data structure
struct TestMessage {
    int id;
    std::string data;
    
    TestMessage(int i = 0, const std::string& d = "") : id(i), data(d) {}
};

// Basic FIFO functionality tests
TEST_F(SDFifoTest, BasicPushPop) {
    CSDFifo<TestMessage> fifo;
    
    // Test empty queue
    TestMessage* msg = fifo.Pop(0);
    EXPECT_EQ(msg, nullptr);
    
    // Test size on empty queue
    EXPECT_EQ(fifo.ReadSize(), 0);
    EXPECT_EQ(fifo.WriteSize(), 0);
}

TEST_F(SDFifoTest, SinglePushPop) {
    CSDFifo<TestMessage> fifo;
    
    // Create test message
    TestMessage* testMsg = new TestMessage(1, "test data");
    
    // Push message
    fifo.Push(testMsg);
    
    // Check sizes
    EXPECT_EQ(fifo.ReadSize(), 1);
    EXPECT_GT(fifo.WriteSize(), 0);
    
    // Pop message
    TestMessage* poppedMsg = fifo.Pop(0);
    EXPECT_NE(poppedMsg, nullptr);
    EXPECT_EQ(poppedMsg->id, 1);
    EXPECT_EQ(poppedMsg->data, "test data");
    EXPECT_EQ(poppedMsg, testMsg);
    
    // Check empty after pop
    EXPECT_EQ(fifo.ReadSize(), 0);
    
    delete testMsg;
}

TEST_F(SDFifoTest, MultiplePushPop) {
    CSDFifo<TestMessage> fifo;
    
    std::vector<TestMessage*> messages;
    
    // Push multiple messages
    for (int i = 0; i < 10; ++i) {
        TestMessage* msg = new TestMessage(i, "data" + std::to_string(i));
        messages.push_back(msg);
        fifo.Push(msg);
    }
    
    // Check size
    EXPECT_EQ(fifo.ReadSize(), 10);
    
    // Pop messages and verify FIFO order
    for (int i = 0; i < 10; ++i) {
        TestMessage* msg = fifo.Pop(0);
        EXPECT_NE(msg, nullptr);
        EXPECT_EQ(msg->id, i);
        EXPECT_EQ(msg->data, "data" + std::to_string(i));
        EXPECT_EQ(msg, messages[i]);
    }
    
    // Check empty
    EXPECT_EQ(fifo.ReadSize(), 0);
    TestMessage* msg = fifo.Pop(0);
    EXPECT_EQ(msg, nullptr);
    
    // Cleanup
    for (auto* m : messages) {
        delete m;
    }
}

TEST_F(SDFifoTest, InterleavedPushPop) {
    CSDFifo<TestMessage> fifo;
    
    // Interleave push and pop operations
    TestMessage* msg1 = new TestMessage(1, "first");
    fifo.Push(msg1);
    
    TestMessage* msg2 = new TestMessage(2, "second");
    fifo.Push(msg2);
    
    // Pop first
    TestMessage* popped1 = fifo.Pop(0);
    EXPECT_EQ(popped1, msg1);
    EXPECT_EQ(fifo.ReadSize(), 1);
    
    // Push another
    TestMessage* msg3 = new TestMessage(3, "third");
    fifo.Push(msg3);
    EXPECT_EQ(fifo.ReadSize(), 2);
    
    // Pop remaining in order
    TestMessage* popped2 = fifo.Pop(0);
    EXPECT_EQ(popped2, msg2);
    
    TestMessage* popped3 = fifo.Pop(0);
    EXPECT_EQ(popped3, msg3);
    
    EXPECT_EQ(fifo.ReadSize(), 0);
    
    delete msg1;
    delete msg2;
    delete msg3;
}

// Timeout tests
TEST_F(SDFifoTest, PopTimeout) {
    CSDFifo<TestMessage> fifo;
    
    // Test immediate timeout (0 ms)
    auto start = std::chrono::high_resolution_clock::now();
    TestMessage* msg = fifo.Pop(0);
    auto duration = std::chrono::high_resolution_clock::now() - start;
    
    EXPECT_EQ(msg, nullptr);
    EXPECT_LT(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(), 50);
}

TEST_F(SDFifoTest, PopWithTimeout) {
    CSDFifo<TestMessage> fifo;
    
    // Test timeout with small delay
    auto start = std::chrono::high_resolution_clock::now();
    TestMessage* msg = fifo.Pop(100); // 100ms timeout
    auto duration = std::chrono::high_resolution_clock::now() - start;
    
    EXPECT_EQ(msg, nullptr);
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    EXPECT_GE(elapsed, 90);  // Should wait approximately 100ms
    EXPECT_LE(elapsed, 200); // Allow some variance
}

// Thread safety tests
TEST_F(SDFifoTest, SingleProducerSingleConsumer) {
    CSDFifo<TestMessage> fifo;
    std::atomic<int> producedCount(0);
    std::atomic<int> consumedCount(0);
    const int messageCount = 1000;
    
    // Producer thread
    std::thread producer([&]() {
        for (int i = 0; i < messageCount; ++i) {
            TestMessage* msg = new TestMessage(i, "data" + std::to_string(i));
            fifo.Push(msg);
            producedCount++;
            // Small delay to allow interleaving
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
    });
    
    // Consumer thread
    std::thread consumer([&]() {
        int lastId = -1;
        while (consumedCount < messageCount) {
            TestMessage* msg = fifo.Pop(10); // 10ms timeout
            if (msg != nullptr) {
                EXPECT_EQ(msg->id, lastId + 1); // Verify order
                lastId = msg->id;
                delete msg;
                consumedCount++;
            }
        }
    });
    
    producer.join();
    consumer.join();
    
    EXPECT_EQ(producedCount, messageCount);
    EXPECT_EQ(consumedCount, messageCount);
    EXPECT_EQ(fifo.ReadSize(), 0);
}

TEST_F(SDFifoTest, MultipleProducersConsumers) {
    CSDFifo<TestMessage> fifo;
    std::atomic<int> totalProduced(0);
    std::atomic<int> totalConsumed(0);
    const int threadsPerType = 3;
    const int messagesPerThread = 100;
    
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;
    
    // Create producer threads
    for (int t = 0; t < threadsPerType; ++t) {
        producers.emplace_back([&, t]() {
            for (int i = 0; i < messagesPerThread; ++i) {
                int id = t * messagesPerThread + i;
                TestMessage* msg = new TestMessage(id, "thread" + std::to_string(t) + "_msg" + std::to_string(i));
                fifo.Push(msg);
                totalProduced++;
                std::this_thread::sleep_for(std::chrono::microseconds(10));
            }
        });
    }
    
    // Create consumer threads
    for (int t = 0; t < threadsPerType; ++t) {
        consumers.emplace_back([&]() {
            while (totalConsumed < threadsPerType * messagesPerThread) {
                TestMessage* msg = fifo.Pop(50); // 50ms timeout
                if (msg != nullptr) {
                    EXPECT_GE(msg->id, 0);
                    EXPECT_LT(msg->id, threadsPerType * messagesPerThread);
                    delete msg;
                    totalConsumed++;
                }
            }
        });
    }
    
    // Wait for all threads
    for (auto& p : producers) {
        p.join();
    }
    for (auto& c : consumers) {
        c.join();
    }
    
    EXPECT_EQ(totalProduced, threadsPerType * messagesPerThread);
    EXPECT_EQ(totalConsumed, threadsPerType * messagesPerThread);
}

// Stress tests
TEST_F(SDFifoTest, HighVolumeStressTest) {
    CSDFifo<TestMessage> fifo;
    const int messageCount = 10000;
    
    // Fill queue with many messages
    for (int i = 0; i < messageCount; ++i) {
        TestMessage* msg = new TestMessage(i, "stress_test_" + std::to_string(i));
        fifo.Push(msg);
    }
    
    EXPECT_EQ(fifo.ReadSize(), messageCount);
    
    // Drain queue and verify order
    for (int i = 0; i < messageCount; ++i) {
        TestMessage* msg = fifo.Pop(0);
        EXPECT_NE(msg, nullptr);
        EXPECT_EQ(msg->id, i);
        delete msg;
    }
    
    EXPECT_EQ(fifo.ReadSize(), 0);
}

TEST_F(SDFifoTest, RapidPushPopCycles) {
    CSDFifo<TestMessage> fifo;
    const int cycles = 1000;
    
    for (int cycle = 0; cycle < cycles; ++cycle) {
        // Push some messages
        std::vector<TestMessage*> messages;
        int msgCount = (cycle % 10) + 1; // 1-10 messages per cycle
        
        for (int i = 0; i < msgCount; ++i) {
            TestMessage* msg = new TestMessage(cycle * 100 + i, "cycle" + std::to_string(cycle));
            messages.push_back(msg);
            fifo.Push(msg);
        }
        
        // Pop all messages
        for (int i = 0; i < msgCount; ++i) {
            TestMessage* msg = fifo.Pop(0);
            EXPECT_NE(msg, nullptr);
            EXPECT_EQ(msg, messages[i]);
            delete msg;
        }
        
        EXPECT_EQ(fifo.ReadSize(), 0);
    }
}

// Performance tests
TEST_F(SDFifoTest, ThroughputTest) {
    CSDFifo<TestMessage> fifo;
    const int messageCount = 100000;
    
    // Measure push throughput
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < messageCount; ++i) {
        TestMessage* msg = new TestMessage(i, "perf_test");
        fifo.Push(msg);
    }
    auto pushTime = std::chrono::high_resolution_clock::now() - start;
    
    // Measure pop throughput
    start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < messageCount; ++i) {
        TestMessage* msg = fifo.Pop(0);
        EXPECT_NE(msg, nullptr);
        delete msg;
    }
    auto popTime = std::chrono::high_resolution_clock::now() - start;
    
    auto pushMicros = std::chrono::duration_cast<std::chrono::microseconds>(pushTime).count();
    auto popMicros = std::chrono::duration_cast<std::chrono::microseconds>(popTime).count();
    
    std::cout << "Push throughput: " << (messageCount * 1000000.0 / pushMicros) << " ops/sec\n";
    std::cout << "Pop throughput: " << (messageCount * 1000000.0 / popMicros) << " ops/sec\n";
    
    // Sanity check - should be reasonably fast
    EXPECT_LT(pushMicros, 5000000); // Less than 5 seconds
    EXPECT_LT(popMicros, 5000000);  // Less than 5 seconds
}

// Memory management tests
TEST_F(SDFifoTest, MemoryLeakPrevention) {
    CSDFifo<TestMessage> fifo;
    
    // Push messages but don't pop all of them
    std::vector<TestMessage*> messages;
    for (int i = 0; i < 100; ++i) {
        TestMessage* msg = new TestMessage(i, "leak_test");
        messages.push_back(msg);
        fifo.Push(msg);
    }
    
    // Pop only half
    for (int i = 0; i < 50; ++i) {
        TestMessage* msg = fifo.Pop(0);
        EXPECT_NE(msg, nullptr);
        delete msg;
    }
    
    // Remaining messages should still be in queue
    EXPECT_EQ(fifo.ReadSize(), 50);
    
    // Clean up remaining messages
    for (int i = 50; i < 100; ++i) {
        TestMessage* msg = fifo.Pop(0);
        EXPECT_NE(msg, nullptr);
        delete msg;
    }
    
    EXPECT_EQ(fifo.ReadSize(), 0);
}

// Edge case tests
TEST_F(SDFifoTest, NullPointerHandling) {
    CSDFifo<TestMessage> fifo;
    
    // Push null pointer (should work)
    fifo.Push(nullptr);
    EXPECT_EQ(fifo.ReadSize(), 1);
    
    // Pop null pointer
    TestMessage* msg = fifo.Pop(0);
    EXPECT_EQ(msg, nullptr);
    EXPECT_EQ(fifo.ReadSize(), 0);
}

TEST_F(SDFifoTest, LargeMessageTest) {
    CSDFifo<TestMessage> fifo;
    
    // Create a large message
    std::string largeData(10000, 'X');
    TestMessage* largeMsg = new TestMessage(999, largeData);
    
    fifo.Push(largeMsg);
    EXPECT_EQ(fifo.ReadSize(), 1);
    
    TestMessage* retrieved = fifo.Pop(0);
    EXPECT_EQ(retrieved, largeMsg);
    EXPECT_EQ(retrieved->id, 999);
    EXPECT_EQ(retrieved->data.length(), 10000);
    EXPECT_EQ(retrieved->data[0], 'X');
    EXPECT_EQ(retrieved->data[9999], 'X');
    
    delete largeMsg;
}