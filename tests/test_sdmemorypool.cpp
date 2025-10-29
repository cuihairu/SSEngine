/**
 * @file test_sdmemorypool.cpp
 * @brief Memory pool functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdmemorypool.h"
#include <vector>
#include <thread>
#include <chrono>

using namespace SSCP;

class SDMemoryPoolTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// CSDVarMemoryPool Tests
TEST_F(SDMemoryPoolTest, VarMemoryPool_Create) {
    CSDVarMemoryPool pool;
    
    // Test successful creation
    EXPECT_TRUE(pool.Create());
    
    // Test with custom page size
    EXPECT_TRUE(pool.Create(0x100000));
}

TEST_F(SDMemoryPoolTest, VarMemoryPool_BasicAllocation) {
    CSDVarMemoryPool pool;
    ASSERT_TRUE(pool.Create());
    
    // Test small allocation
    void* ptr1 = pool.Malloc(32);
    EXPECT_NE(ptr1, nullptr);
    
    // Test medium allocation
    void* ptr2 = pool.Malloc(64);
    EXPECT_NE(ptr2, nullptr);
    
    // Test large allocation
    void* ptr3 = pool.Malloc(256);
    EXPECT_NE(ptr3, nullptr);
    
    // Test pointers are different
    EXPECT_NE(ptr1, ptr2);
    EXPECT_NE(ptr2, ptr3);
    EXPECT_NE(ptr1, ptr3);
    
    // Clean up
    pool.Free(ptr1);
    pool.Free(ptr2);
    pool.Free(ptr3);
}

TEST_F(SDMemoryPoolTest, VarMemoryPool_ZeroAllocation) {
    CSDVarMemoryPool pool;
    ASSERT_TRUE(pool.Create());
    
    // Test zero-size allocation
    void* ptr = pool.Malloc(0);
    EXPECT_EQ(ptr, nullptr);
}

TEST_F(SDMemoryPoolTest, VarMemoryPool_LargeAllocation) {
    CSDVarMemoryPool pool;
    ASSERT_TRUE(pool.Create());
    
    // Test allocation larger than MAX_UNIT_SIZE (128)
    void* ptr = pool.Malloc(512);
    EXPECT_NE(ptr, nullptr);
    
    pool.Free(ptr);
}

TEST_F(SDMemoryPoolTest, VarMemoryPool_AllocationAndFree) {
    CSDVarMemoryPool pool;
    ASSERT_TRUE(pool.Create());
    
    std::vector<void*> ptrs;
    
    // Allocate multiple blocks
    for (int i = 0; i < 100; ++i) {
        void* ptr = pool.Malloc(32 + (i % 64));
        EXPECT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    // Free all blocks
    for (void* ptr : ptrs) {
        pool.Free(ptr);
    }
}

TEST_F(SDMemoryPoolTest, VarMemoryPool_MemoryUsage) {
    CSDVarMemoryPool pool;
    ASSERT_TRUE(pool.Create());
    
    INT32 initialUsage = pool.GetMemUsed();
    EXPECT_GT(initialUsage, 0);
    
    // Allocate some memory
    void* ptr = pool.Malloc(1024);
    EXPECT_NE(ptr, nullptr);
    
    INT32 usageAfterAlloc = pool.GetMemUsed();
    EXPECT_GE(usageAfterAlloc, initialUsage);
    
    pool.Free(ptr);
}

TEST_F(SDMemoryPoolTest, VarMemoryPool_Clear) {
    CSDVarMemoryPool pool;
    ASSERT_TRUE(pool.Create());
    
    // Allocate some memory
    void* ptr1 = pool.Malloc(64);
    void* ptr2 = pool.Malloc(128);
    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    
    // Clear should free all memory
    pool.Clear();
    
    // Memory usage should be reset
    INT32 usageAfterClear = pool.GetMemUsed();
    EXPECT_EQ(usageAfterClear, 0);
}

// CSDFixMemoryPool Tests
TEST_F(SDMemoryPoolTest, FixMemoryPool_Create) {
    CSDFixMemoryPool pool;
    
    // Test successful creation
    EXPECT_TRUE(pool.Create(64));
    
    // Test with custom page size
    EXPECT_TRUE(pool.Create(32, 0x80000));
    
    // Test with zero unit size should fail
    EXPECT_FALSE(pool.Create(0));
}

TEST_F(SDMemoryPoolTest, FixMemoryPool_BasicAllocation) {
    CSDFixMemoryPool pool;
    ASSERT_TRUE(pool.Create(64));
    
    // Test allocation
    void* ptr1 = pool.Malloc();
    EXPECT_NE(ptr1, nullptr);
    
    void* ptr2 = pool.Malloc();
    EXPECT_NE(ptr2, nullptr);
    
    // Test pointers are different
    EXPECT_NE(ptr1, ptr2);
    
    // Clean up
    pool.Free(ptr1);
    pool.Free(ptr2);
}

TEST_F(SDMemoryPoolTest, FixMemoryPool_MultipleAllocations) {
    CSDFixMemoryPool pool;
    ASSERT_TRUE(pool.Create(32));
    
    std::vector<void*> ptrs;
    
    // Allocate multiple blocks
    for (int i = 0; i < 1000; ++i) {
        void* ptr = pool.Malloc();
        EXPECT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    // Free all blocks
    for (void* ptr : ptrs) {
        pool.Free(ptr);
    }
}

TEST_F(SDMemoryPoolTest, FixMemoryPool_FreeAndReuse) {
    CSDFixMemoryPool pool;
    ASSERT_TRUE(pool.Create(64));
    
    // Allocate and free, then allocate again
    void* ptr1 = pool.Malloc();
    EXPECT_NE(ptr1, nullptr);
    
    pool.Free(ptr1);
    
    void* ptr2 = pool.Malloc();
    EXPECT_NE(ptr2, nullptr);
    
    // The freed memory should be reused
    EXPECT_EQ(ptr1, ptr2);
    
    pool.Free(ptr2);
}

TEST_F(SDMemoryPoolTest, FixMemoryPool_MemoryUsage) {
    CSDFixMemoryPool pool;
    ASSERT_TRUE(pool.Create(128));
    
    INT32 initialUsage = pool.GetMemUsed();
    EXPECT_GT(initialUsage, 0);
    
    // Allocate some memory
    void* ptr = pool.Malloc();
    EXPECT_NE(ptr, nullptr);
    
    INT32 usageAfterAlloc = pool.GetMemUsed();
    EXPECT_GE(usageAfterAlloc, initialUsage);
    
    pool.Free(ptr);
}

TEST_F(SDMemoryPoolTest, FixMemoryPool_Clear) {
    CSDFixMemoryPool pool;
    ASSERT_TRUE(pool.Create(64));
    
    // Allocate some memory
    void* ptr1 = pool.Malloc();
    void* ptr2 = pool.Malloc();
    EXPECT_NE(ptr1, nullptr);
    EXPECT_NE(ptr2, nullptr);
    
    // Clear should free all memory
    pool.Clear();
    
    // Memory usage should be reset
    INT32 usageAfterClear = pool.GetMemUsed();
    EXPECT_EQ(usageAfterClear, 0);
}

// Stress Tests
TEST_F(SDMemoryPoolTest, VarMemoryPool_StressTest) {
    CSDVarMemoryPool pool;
    ASSERT_TRUE(pool.Create());
    
    const int numIterations = 10000;
    std::vector<void*> ptrs;
    ptrs.reserve(numIterations);
    
    // Allocate
    for (int i = 0; i < numIterations; ++i) {
        size_t size = 8 + (i % 120); // Various sizes from 8 to 127
        void* ptr = pool.Malloc(size);
        EXPECT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    // Free in random order
    std::random_shuffle(ptrs.begin(), ptrs.end());
    for (void* ptr : ptrs) {
        pool.Free(ptr);
    }
}

TEST_F(SDMemoryPoolTest, FixMemoryPool_StressTest) {
    CSDFixMemoryPool pool;
    ASSERT_TRUE(pool.Create(64));
    
    const int numIterations = 10000;
    std::vector<void*> ptrs;
    ptrs.reserve(numIterations);
    
    // Allocate
    for (int i = 0; i < numIterations; ++i) {
        void* ptr = pool.Malloc();
        EXPECT_NE(ptr, nullptr);
        ptrs.push_back(ptr);
    }
    
    // Free in random order
    std::random_shuffle(ptrs.begin(), ptrs.end());
    for (void* ptr : ptrs) {
        pool.Free(ptr);
    }
}

// Thread Safety Tests (basic - pools are not thread-safe as noted in headers)
TEST_F(SDMemoryPoolTest, VarMemoryPool_DataIntegrity) {
    CSDVarMemoryPool pool;
    ASSERT_TRUE(pool.Create());
    
    // Allocate and write data
    struct TestData {
        int value1;
        float value2;
        char buffer[32];
    };
    
    void* ptr = pool.Malloc(sizeof(TestData));
    EXPECT_NE(ptr, nullptr);
    
    TestData* data = static_cast<TestData*>(ptr);
    data->value1 = 12345;
    data->value2 = 67.89f;
    strcpy(data->buffer, "test data");
    
    // Verify data integrity
    EXPECT_EQ(data->value1, 12345);
    EXPECT_FLOAT_EQ(data->value2, 67.89f);
    EXPECT_STREQ(data->buffer, "test data");
    
    pool.Free(ptr);
}

TEST_F(SDMemoryPoolTest, FixMemoryPool_DataIntegrity) {
    CSDFixMemoryPool pool;
    ASSERT_TRUE(pool.Create(64));
    
    // Allocate and write data
    void* ptr = pool.Malloc();
    EXPECT_NE(ptr, nullptr);
    
    int* data = static_cast<int*>(ptr);
    *data = 98765;
    
    // Verify data integrity
    EXPECT_EQ(*data, 98765);
    
    pool.Free(ptr);
}

// Performance comparison test
TEST_F(SDMemoryPoolTest, PerformanceComparison) {
    const int numAllocations = 1000;
    
    // Test standard malloc/free
    auto start = std::chrono::high_resolution_clock::now();
    std::vector<void*> stdPtrs;
    for (int i = 0; i < numAllocations; ++i) {
        void* ptr = malloc(64);
        stdPtrs.push_back(ptr);
    }
    for (void* ptr : stdPtrs) {
        free(ptr);
    }
    auto stdTime = std::chrono::high_resolution_clock::now() - start;
    
    // Test fixed memory pool
    start = std::chrono::high_resolution_clock::now();
    CSDFixMemoryPool pool;
    ASSERT_TRUE(pool.Create(64));
    std::vector<void*> poolPtrs;
    for (int i = 0; i < numAllocations; ++i) {
        void* ptr = pool.Malloc();
        poolPtrs.push_back(ptr);
    }
    for (void* ptr : poolPtrs) {
        pool.Free(ptr);
    }
    auto poolTime = std::chrono::high_resolution_clock::now() - start;
    
    // Pool should be faster or at least competitive
    std::cout << "Standard malloc/free: " << 
        std::chrono::duration_cast<std::chrono::microseconds>(stdTime).count() << " μs\n";
    std::cout << "Memory pool: " << 
        std::chrono::duration_cast<std::chrono::microseconds>(poolTime).count() << " μs\n";
}