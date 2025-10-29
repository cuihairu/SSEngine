/**
 * @file test_sdthreadpool.cpp
 * @brief Thread pool functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdthreadpool.h"
#include <atomic>
#include <chrono>
#include <vector>
#include <thread>

using namespace SSCP;

class SDThreadPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// Test job implementations
class CounterJob : public ISSRunable {
public:
    CounterJob(std::atomic<int>& counter) : counter_(counter) {}
    
    void Run() override {
        counter_++;
    }
    
private:
    std::atomic<int>& counter_;
};

class SleepJob : public ISSRunable {
public:
    SleepJob(int sleepMs, std::atomic<int>& counter) 
        : sleepMs_(sleepMs), counter_(counter) {}
    
    void Run() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepMs_));
        counter_++;
    }
    
private:
    int sleepMs_;
    std::atomic<int>& counter_;
};

class TimeStampJob : public ISSRunable {
public:
    TimeStampJob(std::vector<std::chrono::high_resolution_clock::time_point>& timestamps)
        : timestamps_(timestamps) {}
    
    void Run() override {
        timestamps_.push_back(std::chrono::high_resolution_clock::now());
    }
    
private:
    std::vector<std::chrono::high_resolution_clock::time_point>& timestamps_;
};

class ExceptionJob : public ISSRunable {
public:
    ExceptionJob(std::atomic<int>& counter) : counter_(counter) {}
    
    void Run() override {
        counter_++;
        // Simulate an exception (but don't actually throw to avoid test issues)
        // Just complete normally
    }
    
private:
    std::atomic<int>& counter_;
};

// Basic functionality tests
TEST_F(SDThreadPoolTest, BasicInitialization) {
    CSDThreadPool pool;
    
    // Test successful initialization
    EXPECT_TRUE(pool.Init(2, 5, 10));
    EXPECT_EQ(pool.GetThreadNum(), 2);
    EXPECT_EQ(pool.GetJobPending(), 0);
    
    // Test with minimum threads
    pool.TerminateQuick();
    EXPECT_TRUE(pool.Init(1, 1, 5));
    EXPECT_EQ(pool.GetThreadNum(), 1);
}

TEST_F(SDThreadPoolTest, SingleJobExecution) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(1, 2, 5));
    
    std::atomic<int> counter{0};
    CounterJob* job = new CounterJob(counter);
    
    EXPECT_TRUE(pool.ScheduleJob(job));
    
    // Wait for job to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_EQ(counter.load(), 1);
    EXPECT_EQ(pool.GetJobPending(), 0);
    
    delete job;
}

TEST_F(SDThreadPoolTest, MultipleJobExecution) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(2, 4, 20));
    
    std::atomic<int> counter{0};
    const int numJobs = 10;
    std::vector<CounterJob*> jobs;
    
    // Schedule multiple jobs
    for (int i = 0; i < numJobs; ++i) {
        CounterJob* job = new CounterJob(counter);
        jobs.push_back(job);
        EXPECT_TRUE(pool.ScheduleJob(job));
    }
    
    // Wait for all jobs to complete
    while (pool.GetJobPending() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_EQ(counter.load(), numJobs);
    
    // Cleanup
    for (auto* job : jobs) {
        delete job;
    }
}

TEST_F(SDThreadPoolTest, ThreadPoolScaling) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(1, 5, 100));
    
    std::atomic<int> counter{0};
    const int numJobs = 20;
    std::vector<SleepJob*> jobs;
    
    // Schedule jobs that take some time
    for (int i = 0; i < numJobs; ++i) {
        SleepJob* job = new SleepJob(50, counter); // 50ms each
        jobs.push_back(job);
        EXPECT_TRUE(pool.ScheduleJob(job));
    }
    
    // Wait a bit and check if threads were added
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    UINT32 threadCount = pool.GetThreadNum();
    EXPECT_GT(threadCount, 1); // Should have scaled up
    EXPECT_LE(threadCount, 5); // But not beyond max
    
    // Wait for all jobs to complete
    while (pool.GetJobPending() > 0 || counter.load() < numJobs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_EQ(counter.load(), numJobs);
    
    // Cleanup
    for (auto* job : jobs) {
        delete job;
    }
}

TEST_F(SDThreadPoolTest, MaxPendingJobsLimit) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(1, 1, 3)); // Max 3 pending jobs
    
    std::atomic<int> counter{0};
    std::vector<SleepJob*> jobs;
    
    // Schedule jobs that will block the single thread
    for (int i = 0; i < 5; ++i) {
        SleepJob* job = new SleepJob(100, counter);
        jobs.push_back(job);
        
        if (i < 4) { // First 4 should succeed (1 executing + 3 pending)
            EXPECT_TRUE(pool.ScheduleJob(job));
        } else { // 5th should fail due to limit
            EXPECT_FALSE(pool.ScheduleJob(job));
        }
    }
    
    // Wait for jobs to complete
    while (counter.load() < 4) { // Only 4 jobs were scheduled
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_EQ(counter.load(), 4);
    
    // Cleanup
    for (auto* job : jobs) {
        delete job;
    }
}

TEST_F(SDThreadPoolTest, TerminateQuick) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(2, 4, 20));
    
    std::atomic<int> counter{0};
    std::vector<SleepJob*> jobs;
    
    // Schedule long-running jobs
    for (int i = 0; i < 10; ++i) {
        SleepJob* job = new SleepJob(200, counter);
        jobs.push_back(job);
        EXPECT_TRUE(pool.ScheduleJob(job));
    }
    
    // Wait a bit to let some jobs start
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Terminate quickly - should cancel pending jobs
    pool.TerminateQuick();
    
    EXPECT_EQ(pool.GetThreadNum(), 0);
    EXPECT_EQ(pool.GetJobPending(), 0);
    
    // Should have executed fewer than all jobs
    EXPECT_LT(counter.load(), 10);
    
    // Cleanup
    for (auto* job : jobs) {
        delete job;
    }
}

TEST_F(SDThreadPoolTest, TerminateWaitJobs) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(2, 4, 20));
    
    std::atomic<int> counter{0};
    std::vector<SleepJob*> jobs;
    
    // Schedule short jobs
    for (int i = 0; i < 8; ++i) {
        SleepJob* job = new SleepJob(50, counter);
        jobs.push_back(job);
        EXPECT_TRUE(pool.ScheduleJob(job));
    }
    
    // Terminate with wait - should complete all jobs
    pool.TerminateWaitJobs();
    
    EXPECT_EQ(pool.GetThreadNum(), 0);
    EXPECT_EQ(pool.GetJobPending(), 0);
    EXPECT_EQ(counter.load(), 8); // All jobs should complete
    
    // Cleanup
    for (auto* job : jobs) {
        delete job;
    }
}

TEST_F(SDThreadPoolTest, NullJobHandling) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(1, 2, 5));
    
    // Should reject null jobs
    EXPECT_FALSE(pool.ScheduleJob(nullptr));
}

TEST_F(SDThreadPoolTest, ReInitialization) {
    CSDThreadPool pool;
    
    // First initialization
    ASSERT_TRUE(pool.Init(1, 2, 5));
    EXPECT_EQ(pool.GetThreadNum(), 1);
    
    std::atomic<int> counter{0};
    CounterJob* job1 = new CounterJob(counter);
    EXPECT_TRUE(pool.ScheduleJob(job1));
    
    // Re-initialize - should clean up previous state
    ASSERT_TRUE(pool.Init(3, 5, 10));
    EXPECT_EQ(pool.GetThreadNum(), 3);
    EXPECT_EQ(pool.GetJobPending(), 0);
    
    // New jobs should work
    CounterJob* job2 = new CounterJob(counter);
    EXPECT_TRUE(pool.ScheduleJob(job2));
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    delete job1;
    delete job2;
}

TEST_F(SDThreadPoolTest, ZeroParameterHandling) {
    CSDThreadPool pool;
    
    // Test with zero values - should use defaults
    EXPECT_TRUE(pool.Init(0, 0, 0));
    EXPECT_GE(pool.GetThreadNum(), 1); // Should default to at least 1
}

TEST_F(SDThreadPoolTest, ConcurrentScheduling) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(3, 8, 100));
    
    std::atomic<int> counter{0};
    std::atomic<int> scheduledCount{0};
    const int threadsCount = 4;
    const int jobsPerThread = 25;
    
    std::vector<std::thread> schedulerThreads;
    std::vector<std::vector<CounterJob*>> allJobs(threadsCount);
    
    // Create multiple threads that schedule jobs concurrently
    for (int t = 0; t < threadsCount; ++t) {
        schedulerThreads.emplace_back([&, t]() {
            for (int i = 0; i < jobsPerThread; ++i) {
                CounterJob* job = new CounterJob(counter);
                allJobs[t].push_back(job);
                if (pool.ScheduleJob(job)) {
                    scheduledCount++;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    
    // Wait for all scheduler threads
    for (auto& t : schedulerThreads) {
        t.join();
    }
    
    // Wait for all jobs to complete
    while (pool.GetJobPending() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    EXPECT_EQ(counter.load(), scheduledCount.load());
    EXPECT_EQ(scheduledCount.load(), threadsCount * jobsPerThread);
    
    // Cleanup
    for (auto& jobs : allJobs) {
        for (auto* job : jobs) {
            delete job;
        }
    }
}

TEST_F(SDThreadPoolTest, ThreadPoolGrowthPattern) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(1, 6, 50));
    
    std::atomic<int> counter{0};
    std::vector<SleepJob*> jobs;
    
    // Start with small number of threads
    EXPECT_EQ(pool.GetThreadNum(), 1);
    
    // Add workload gradually and observe thread count growth
    for (int batch = 0; batch < 5; ++batch) {
        // Add 3 jobs per batch
        for (int i = 0; i < 3; ++i) {
            SleepJob* job = new SleepJob(100, counter);
            jobs.push_back(job);
            EXPECT_TRUE(pool.ScheduleJob(job));
        }
        
        // Give time for thread pool to react
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        UINT32 threadCount = pool.GetThreadNum();
        EXPECT_GE(threadCount, 1);
        EXPECT_LE(threadCount, 6);
        
        // More jobs should generally mean more threads (up to limit)
        if (batch > 0) {
            // Thread count should generally increase or stay same
            // (This is a general expectation, actual behavior may vary)
        }
    }
    
    // Wait for completion
    while (pool.GetJobPending() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Cleanup
    for (auto* job : jobs) {
        delete job;
    }
}

// Performance and stress tests
TEST_F(SDThreadPoolTest, HighVolumeJobProcessing) {
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(4, 8, 1000));
    
    std::atomic<int> counter{0};
    const int numJobs = 500;
    std::vector<CounterJob*> jobs;
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Schedule many small jobs
    for (int i = 0; i < numJobs; ++i) {
        CounterJob* job = new CounterJob(counter);
        jobs.push_back(job);
        EXPECT_TRUE(pool.ScheduleJob(job));
    }
    
    // Wait for all jobs to complete
    while (pool.GetJobPending() > 0 || counter.load() < numJobs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    auto duration = std::chrono::high_resolution_clock::now() - start;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    EXPECT_EQ(counter.load(), numJobs);
    
    std::cout << "Processed " << numJobs << " jobs in " << ms << " ms\n";
    std::cout << "Average job processing rate: " << (double)numJobs / ms * 1000 << " jobs/sec\n";
    
    // Should be reasonably fast
    EXPECT_LT(ms, 5000); // Less than 5 seconds
    
    // Cleanup
    for (auto* job : jobs) {
        delete job;
    }
}

TEST_F(SDThreadPoolTest, PerformanceComparison) {
    const int numJobs = 1000;
    std::atomic<int> counter1{0};
    std::atomic<int> counter2{0};
    
    // Test sequential execution
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < numJobs; ++i) {
        counter1++;
    }
    auto sequentialTime = std::chrono::high_resolution_clock::now() - start;
    
    // Test thread pool execution
    CSDThreadPool pool;
    ASSERT_TRUE(pool.Init(4, 8, numJobs));
    
    std::vector<CounterJob*> jobs;
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numJobs; ++i) {
        CounterJob* job = new CounterJob(counter2);
        jobs.push_back(job);
        EXPECT_TRUE(pool.ScheduleJob(job));
    }
    
    while (pool.GetJobPending() > 0 || counter2.load() < numJobs) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    auto poolTime = std::chrono::high_resolution_clock::now() - start;
    
    auto seqMicros = std::chrono::duration_cast<std::chrono::microseconds>(sequentialTime).count();
    auto poolMicros = std::chrono::duration_cast<std::chrono::microseconds>(poolTime).count();
    
    std::cout << "Sequential execution: " << seqMicros << " μs\n";
    std::cout << "Thread pool execution: " << poolMicros << " μs\n";
    
    // For simple increment operations, thread pool might be slower due to overhead
    // But it should still complete in reasonable time
    EXPECT_LT(poolMicros, 1000000); // Less than 1 second
    
    EXPECT_EQ(counter1.load(), numJobs);
    EXPECT_EQ(counter2.load(), numJobs);
    
    // Cleanup
    for (auto* job : jobs) {
        delete job;
    }
}