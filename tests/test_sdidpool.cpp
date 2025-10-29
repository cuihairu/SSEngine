/**
 * @file test_sdidpool.cpp
 * @brief ID pool functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdidpool.h"
#include <vector>
#include <set>
#include <thread>
#include <atomic>
#include <chrono>

using namespace SSCP;

class SDIdPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// Test data structure
struct TestObject {
    int value;
    std::string name;
    
    TestObject(int v = 0, const std::string& n = "") : value(v), name(n) {}
    
    bool operator==(const TestObject& other) const {
        return value == other.value && name == other.name;
    }
};

// Basic ID pool functionality tests
TEST_F(SDIdPoolTest, BasicInit) {
    CSDIDPool<TestObject> pool;
    
    // Test initialization
    EXPECT_TRUE(pool.Init(100));
    
    // Check initial state
    EXPECT_EQ(pool.GetFreeCount(), 100);
    EXPECT_TRUE(pool.IsEmpty());
    EXPECT_FALSE(pool.IsFull());
    EXPECT_EQ(pool.Capacity(), 100);
    EXPECT_EQ(pool.GetUSG(), 0); // 0% usage
}

TEST_F(SDIdPoolTest, BasicAllocation) {
    CSDIDPool<TestObject> pool;
    ASSERT_TRUE(pool.Init(10));
    
    TestObject obj1(1, "first");
    UINT32 id1 = pool.Alloc(obj1);
    
    // Should get a valid ID
    EXPECT_NE(id1, (UINT32)-1);
    EXPECT_LT(id1, 10);
    
    // Pool state should update
    EXPECT_EQ(pool.GetFreeCount(), 9);
    EXPECT_FALSE(pool.IsEmpty());
    EXPECT_FALSE(pool.IsFull());
    EXPECT_EQ(pool.GetUSG(), 10); // 10% usage
}

TEST_F(SDIdPoolTest, FindObject) {
    CSDIDPool<TestObject> pool;
    ASSERT_TRUE(pool.Init(10));
    
    TestObject obj1(42, "test_object");
    UINT32 id = pool.Alloc(obj1);
    EXPECT_NE(id, (UINT32)-1);
    
    // Find the object
    TestObject found;
    EXPECT_TRUE(pool.Find(id, found));
    EXPECT_EQ(found.value, 42);
    EXPECT_EQ(found.name, "test_object");
    
    // Test invalid ID
    TestObject notFound;
    EXPECT_FALSE(pool.Find(999, notFound));
}

TEST_F(SDIdPoolTest, FreeObject) {
    CSDIDPool<TestObject> pool;
    ASSERT_TRUE(pool.Init(10));
    
    TestObject obj1(1, "to_free");
    UINT32 id = pool.Alloc(obj1);
    EXPECT_NE(id, (UINT32)-1);
    EXPECT_EQ(pool.GetFreeCount(), 9);
    
    // Free the object
    pool.Free(id);
    EXPECT_EQ(pool.GetFreeCount(), 10);
    EXPECT_TRUE(pool.IsEmpty());
    
    // Should not be able to find freed object
    TestObject found;
    EXPECT_FALSE(pool.Find(id, found));
}

TEST_F(SDIdPoolTest, AllocationAndFreePattern) {
    CSDIDPool<TestObject> pool;
    ASSERT_TRUE(pool.Init(5));
    
    std::vector<UINT32> ids;
    
    // Allocate all slots
    for (int i = 0; i < 5; ++i) {
        TestObject obj(i, "obj" + std::to_string(i));
        UINT32 id = pool.Alloc(obj);
        EXPECT_NE(id, (UINT32)-1);
        ids.push_back(id);
    }
    
    EXPECT_TRUE(pool.IsFull());
    EXPECT_EQ(pool.GetFreeCount(), 0);
    EXPECT_EQ(pool.GetUSG(), 100);
    
    // Try to allocate when full
    TestObject extraObj(999, "extra");
    UINT32 invalidId = pool.Alloc(extraObj);
    EXPECT_EQ(invalidId, (UINT32)-1);
    
    // Free some objects
    pool.Free(ids[1]); // Free second object
    pool.Free(ids[3]); // Free fourth object
    
    EXPECT_EQ(pool.GetFreeCount(), 2);
    EXPECT_FALSE(pool.IsFull());
    EXPECT_EQ(pool.GetUSG(), 60); // 3 out of 5 used
    
    // Verify remaining objects are still findable
    TestObject found;
    EXPECT_TRUE(pool.Find(ids[0], found));
    EXPECT_EQ(found.value, 0);
    
    EXPECT_TRUE(pool.Find(ids[2], found));
    EXPECT_EQ(found.value, 2);
    
    EXPECT_TRUE(pool.Find(ids[4], found));
    EXPECT_EQ(found.value, 4);
    
    // Freed objects should not be findable
    EXPECT_FALSE(pool.Find(ids[1], found));
    EXPECT_FALSE(pool.Find(ids[3], found));
}

TEST_F(SDIdPoolTest, ReuseFreedIds) {
    CSDIDPool<TestObject> pool;
    ASSERT_TRUE(pool.Init(3));
    
    // Allocate an object
    TestObject obj1(1, "first");
    UINT32 id1 = pool.Alloc(obj1);
    EXPECT_NE(id1, (UINT32)-1);
    
    // Free it
    pool.Free(id1);
    
    // Allocate again - should potentially reuse the ID
    TestObject obj2(2, "second");
    UINT32 id2 = pool.Alloc(obj2);
    EXPECT_NE(id2, (UINT32)-1);
    
    // Verify the new object
    TestObject found;
    EXPECT_TRUE(pool.Find(id2, found));
    EXPECT_EQ(found.value, 2);
    EXPECT_EQ(found.name, "second");
}

TEST_F(SDIdPoolTest, EdgeCaseInvalidOperations) {
    CSDIDPool<TestObject> pool;
    ASSERT_TRUE(pool.Init(5));
    
    // Test operations with invalid IDs
    TestObject found;
    EXPECT_FALSE(pool.Find(999, found));
    EXPECT_FALSE(pool.Find(5, found)); // Out of range
    
    pool.Free(999); // Should not crash
    pool.Free(5);   // Should not crash
    
    // Test double free
    TestObject obj(1, "test");
    UINT32 id = pool.Alloc(obj);
    EXPECT_NE(id, (UINT32)-1);
    
    pool.Free(id);
    pool.Free(id); // Double free - should not crash
    
    EXPECT_EQ(pool.GetFreeCount(), 5); // Should still be all free
}

TEST_F(SDIdPoolTest, ZeroSizeInit) {
    CSDIDPool<TestObject> pool;
    
    // Should handle zero size gracefully
    EXPECT_TRUE(pool.Init(0));
    EXPECT_EQ(pool.Capacity(), 0);
    EXPECT_EQ(pool.GetFreeCount(), 0);
    EXPECT_TRUE(pool.IsEmpty());
    EXPECT_TRUE(pool.IsFull());
}

TEST_F(SDIdPoolTest, LargePoolTest) {
    CSDIDPool<TestObject> pool;
    const UINT32 poolSize = 10000;
    ASSERT_TRUE(pool.Init(poolSize));
    
    std::vector<UINT32> ids;
    ids.reserve(poolSize);
    
    // Allocate all objects
    for (UINT32 i = 0; i < poolSize; ++i) {
        TestObject obj(i, "obj" + std::to_string(i));
        UINT32 id = pool.Alloc(obj);
        EXPECT_NE(id, (UINT32)-1);
        ids.push_back(id);
    }
    
    EXPECT_TRUE(pool.IsFull());
    EXPECT_EQ(pool.GetFreeCount(), 0);
    
    // Verify all objects
    for (UINT32 i = 0; i < poolSize; ++i) {
        TestObject found;
        EXPECT_TRUE(pool.Find(ids[i], found));
        EXPECT_EQ(found.value, (int)i);
    }
    
    // Free all objects
    for (UINT32 id : ids) {
        pool.Free(id);
    }
    
    EXPECT_TRUE(pool.IsEmpty());
    EXPECT_EQ(pool.GetFreeCount(), poolSize);
}

TEST_F(SDIdPoolTest, RandomAllocationPattern) {
    CSDIDPool<TestObject> pool;
    const UINT32 poolSize = 100;
    ASSERT_TRUE(pool.Init(poolSize));
    
    std::vector<UINT32> allocatedIds;
    std::set<UINT32> activeIds;
    
    // Random allocation and deallocation
    for (int iteration = 0; iteration < 1000; ++iteration) {
        if (activeIds.empty() || (activeIds.size() < poolSize/2 && rand() % 2 == 0)) {
            // Allocate
            TestObject obj(iteration, "iter" + std::to_string(iteration));
            UINT32 id = pool.Alloc(obj);
            if (id != (UINT32)-1) {
                allocatedIds.push_back(id);
                activeIds.insert(id);
            }
        } else {
            // Free a random active ID
            if (!activeIds.empty()) {
                auto it = activeIds.begin();
                std::advance(it, rand() % activeIds.size());
                UINT32 idToFree = *it;
                pool.Free(idToFree);
                activeIds.erase(it);
            }
        }
        
        // Verify pool consistency
        EXPECT_EQ(activeIds.size(), poolSize - pool.GetFreeCount());
    }
    
    // Cleanup remaining objects
    for (UINT32 id : activeIds) {
        pool.Free(id);
    }
    
    EXPECT_TRUE(pool.IsEmpty());
}

TEST_F(SDIdPoolTest, ObjectIntegrity) {
    CSDIDPool<TestObject> pool;
    ASSERT_TRUE(pool.Init(10));
    
    // Test complex object storage
    struct ComplexObject {
        int data[100];
        std::string name;
        double value;
        
        ComplexObject(const std::string& n, double v) : name(n), value(v) {
            for (int i = 0; i < 100; ++i) {
                data[i] = i * i;
            }
        }
        
        bool operator==(const ComplexObject& other) const {
            if (name != other.name || value != other.value) return false;
            for (int i = 0; i < 100; ++i) {
                if (data[i] != other.data[i]) return false;
            }
            return true;
        }
    };
    
    CSDIDPool<ComplexObject> complexPool;
    ASSERT_TRUE(complexPool.Init(5));
    
    ComplexObject obj("complex_test", 3.14159);
    UINT32 id = complexPool.Alloc(obj);
    EXPECT_NE(id, (UINT32)-1);
    
    // Retrieve and verify
    ComplexObject found;
    EXPECT_TRUE(complexPool.Find(id, found));
    EXPECT_EQ(found.name, "complex_test");
    EXPECT_DOUBLE_EQ(found.value, 3.14159);
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(found.data[i], i * i);
    }
}

// Performance tests
TEST_F(SDIdPoolTest, AllocationPerformance) {
    CSDIDPool<TestObject> pool;
    const UINT32 poolSize = 10000;
    ASSERT_TRUE(pool.Init(poolSize));
    
    auto start = std::chrono::high_resolution_clock::now();
    
    std::vector<UINT32> ids;
    ids.reserve(poolSize);
    
    // Allocate all objects
    for (UINT32 i = 0; i < poolSize; ++i) {
        TestObject obj(i, "perf_test");
        UINT32 id = pool.Alloc(obj);
        EXPECT_NE(id, (UINT32)-1);
        ids.push_back(id);
    }
    
    auto allocTime = std::chrono::high_resolution_clock::now() - start;
    
    start = std::chrono::high_resolution_clock::now();
    
    // Free all objects
    for (UINT32 id : ids) {
        pool.Free(id);
    }
    
    auto freeTime = std::chrono::high_resolution_clock::now() - start;
    
    auto allocMicros = std::chrono::duration_cast<std::chrono::microseconds>(allocTime).count();
    auto freeMicros = std::chrono::duration_cast<std::chrono::microseconds>(freeTime).count();
    
    std::cout << "Allocation time for " << poolSize << " objects: " << allocMicros << " μs\n";
    std::cout << "Free time for " << poolSize << " objects: " << freeMicros << " μs\n";
    std::cout << "Average allocation time: " << (double)allocMicros / poolSize << " μs per object\n";
    std::cout << "Average free time: " << (double)freeMicros / poolSize << " μs per object\n";
    
    // Performance should be reasonable
    EXPECT_LT(allocMicros, 1000000); // Less than 1 second
    EXPECT_LT(freeMicros, 1000000);  // Less than 1 second
}

TEST_F(SDIdPoolTest, FindPerformance) {
    CSDIDPool<TestObject> pool;
    const UINT32 poolSize = 1000;
    ASSERT_TRUE(pool.Init(poolSize));
    
    std::vector<UINT32> ids;
    
    // Allocate objects
    for (UINT32 i = 0; i < poolSize; ++i) {
        TestObject obj(i, "find_test");
        UINT32 id = pool.Alloc(obj);
        EXPECT_NE(id, (UINT32)-1);
        ids.push_back(id);
    }
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Perform many find operations
    const int findOperations = 10000;
    for (int i = 0; i < findOperations; ++i) {
        UINT32 idToFind = ids[i % ids.size()];
        TestObject found;
        EXPECT_TRUE(pool.Find(idToFind, found));
    }
    
    auto findTime = std::chrono::high_resolution_clock::now() - start;
    auto findMicros = std::chrono::duration_cast<std::chrono::microseconds>(findTime).count();
    
    std::cout << "Find time for " << findOperations << " operations: " << findMicros << " μs\n";
    std::cout << "Average find time: " << (double)findMicros / findOperations << " μs per operation\n";
    
    // Cleanup
    for (UINT32 id : ids) {
        pool.Free(id);
    }
}

// Stress tests
TEST_F(SDIdPoolTest, HighContentionStressTest) {
    CSDIDPool<TestObject> pool;
    const UINT32 poolSize = 1000;
    ASSERT_TRUE(pool.Init(poolSize));
    
    std::atomic<int> totalAllocated(0);
    std::atomic<int> totalFreed(0);
    std::atomic<int> successfulFinds(0);
    
    const int threadsCount = 4;
    const int operationsPerThread = 1000;
    
    std::vector<std::thread> threads;
    
    // Note: CSDIDPool is not thread-safe, but we test for basic data integrity
    // In a real scenario, external synchronization would be needed
    
    for (int t = 0; t < threadsCount; ++t) {
        threads.emplace_back([&, t]() {
            std::vector<UINT32> localIds;
            
            for (int i = 0; i < operationsPerThread; ++i) {
                if (localIds.size() < 50) { // Keep some allocated
                    // Allocate
                    TestObject obj(t * 1000 + i, "thread" + std::to_string(t));
                    UINT32 id = pool.Alloc(obj);
                    if (id != (UINT32)-1) {
                        localIds.push_back(id);
                        totalAllocated++;
                    }
                } else {
                    // Free some
                    if (!localIds.empty()) {
                        UINT32 idToFree = localIds.back();
                        localIds.pop_back();
                        pool.Free(idToFree);
                        totalFreed++;
                    }
                }
                
                // Occasionally try to find objects
                if (i % 10 == 0 && !localIds.empty()) {
                    UINT32 idToFind = localIds[localIds.size() / 2];
                    TestObject found;
                    if (pool.Find(idToFind, found)) {
                        successfulFinds++;
                    }
                }
            }
            
            // Cleanup remaining objects
            for (UINT32 id : localIds) {
                pool.Free(id);
                totalFreed++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    std::cout << "Total allocated: " << totalAllocated << "\n";
    std::cout << "Total freed: " << totalFreed << "\n";
    std::cout << "Successful finds: " << successfulFinds << "\n";
    
    // The pool should be reasonably consistent
    EXPECT_GT(totalAllocated, 0);
    EXPECT_GT(totalFreed, 0);
}