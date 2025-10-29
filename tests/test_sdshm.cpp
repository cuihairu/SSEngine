/**
 * @file test_sdshm.cpp
 * @brief Shared memory functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdshm.h"
#include <string>
#include <vector>
#include <cstring>
#include <thread>
#include <chrono>

using namespace SSCP;

class SDShmTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
        usedKeys.clear();
    }

    void TearDown() override {
        // Cleanup any shared memory segments created during tests
        for (INT32 key : usedKeys) {
            SGSYS_DeleteShm(key);
        }
        usedKeys.clear();
    }
    
    // Helper to get unique keys for testing
    INT32 getUniqueKey() {
        static INT32 keyCounter = 1000;
        INT32 key = keyCounter++;
        usedKeys.push_back(key);
        return key;
    }
    
private:
    std::vector<INT32> usedKeys;
};

// Test data structures
struct TestData {
    int magic;
    char message[256];
    double value;
    
    TestData() : magic(0x12345678), value(0.0) {
        memset(message, 0, sizeof(message));
    }
    
    TestData(const char* msg, double val) : magic(0x12345678), value(val) {
        strncpy(message, msg, sizeof(message) - 1);
        message[sizeof(message) - 1] = '\0';
    }
    
    bool isValid() const {
        return magic == 0x12345678;
    }
};

// Basic functionality tests
TEST_F(SDShmTest, CreateBasicSharedMemory) {
    INT32 key = getUniqueKey();
    UINT32 size = 4096;
    INT32 errCode = 0;
    
    // Create shared memory
    void* shmPtr = SGSYS_NewShm(key, size, &errCode);
    
    EXPECT_NE(shmPtr, nullptr);
    EXPECT_EQ(errCode, SDSHM_SUCCESS);
    
    if (shmPtr != nullptr) {
        // Write some data to verify it's writable
        memset(shmPtr, 0xAA, 100);
        
        // Verify the data
        unsigned char* data = static_cast<unsigned char*>(shmPtr);
        for (int i = 0; i < 100; ++i) {
            EXPECT_EQ(data[i], 0xAA);
        }
    }
    
    // Clean up
    INT32 deleteResult = SGSYS_DeleteShm(key);
    EXPECT_EQ(deleteResult, 1);
}

TEST_F(SDShmTest, InvalidParameters) {
    INT32 key = getUniqueKey();
    INT32 errCode = 0;
    
    // Test zero size
    void* shmPtr = SGSYS_NewShm(key, 0, &errCode);
    EXPECT_EQ(shmPtr, nullptr);
    EXPECT_EQ(errCode, SDSHM_INVALIDPARAM);
    
    // Test null error pointer (should not crash)
    shmPtr = SGSYS_NewShm(key, 1024, nullptr);
    // Should either succeed or fail gracefully
}

TEST_F(SDShmTest, DuplicateKeyHandling) {
    INT32 key = getUniqueKey();
    UINT32 size = 1024;
    INT32 errCode1 = 0, errCode2 = 0;
    
    // Create first shared memory
    void* shmPtr1 = SGSYS_NewShm(key, size, &errCode1);
    EXPECT_NE(shmPtr1, nullptr);
    EXPECT_EQ(errCode1, SDSHM_SUCCESS);
    
    // Try to create with same key - should fail
    void* shmPtr2 = SGSYS_NewShm(key, size, &errCode2);
    EXPECT_EQ(shmPtr2, nullptr);
    EXPECT_EQ(errCode2, SDSHM_ALREADYINUSE);
    
    // Clean up
    SGSYS_DeleteShm(key);
}

TEST_F(SDShmTest, StructuredDataStorage) {
    INT32 key = getUniqueKey();
    UINT32 size = sizeof(TestData);
    INT32 errCode = 0;
    
    // Create shared memory for structured data
    void* shmPtr = SGSYS_NewShm(key, size, &errCode);
    EXPECT_NE(shmPtr, nullptr);
    EXPECT_EQ(errCode, SDSHM_SUCCESS);
    
    if (shmPtr != nullptr) {
        // Place structured data in shared memory
        TestData* testData = new(shmPtr) TestData("Hello Shared Memory", 3.14159);
        
        // Verify data integrity
        EXPECT_TRUE(testData->isValid());
        EXPECT_STREQ(testData->message, "Hello Shared Memory");
        EXPECT_DOUBLE_EQ(testData->value, 3.14159);
        
        // Modify data
        strcpy(testData->message, "Modified Data");
        testData->value = 2.71828;
        
        // Verify modifications
        EXPECT_STREQ(testData->message, "Modified Data");
        EXPECT_DOUBLE_EQ(testData->value, 2.71828);
        EXPECT_TRUE(testData->isValid());
    }
    
    // Clean up
    SGSYS_DeleteShm(key);
}

TEST_F(SDShmTest, LargeMemoryAllocation) {
    INT32 key = getUniqueKey();
    UINT32 size = 1024 * 1024; // 1MB
    INT32 errCode = 0;
    
    // Create large shared memory
    void* shmPtr = SGSYS_NewShm(key, size, &errCode);
    
    // This test might fail on systems with limited shared memory
    if (shmPtr != nullptr) {
        EXPECT_EQ(errCode, SDSHM_SUCCESS);
        
        // Write pattern to large memory area
        char* data = static_cast<char*>(shmPtr);
        
        // Write pattern every 4KB
        for (UINT32 i = 0; i < size; i += 4096) {
            if (i + 4 < size) {
                *(UINT32*)(data + i) = i;
            }
        }
        
        // Verify pattern
        for (UINT32 i = 0; i < size; i += 4096) {
            if (i + 4 < size) {
                EXPECT_EQ(*(UINT32*)(data + i), i);
            }
        }
        
        SGSYS_DeleteShm(key);
    } else {
        // If allocation failed, it should be due to system limits
        EXPECT_EQ(errCode, SDSHM_OVERSIZE);
        std::cout << "Large memory allocation test skipped due to system limits\n";
    }
}

TEST_F(SDShmTest, MultipleSharedMemorySegments) {
    const int numSegments = 5;
    std::vector<INT32> keys;
    std::vector<void*> pointers;
    
    // Create multiple shared memory segments
    for (int i = 0; i < numSegments; ++i) {
        INT32 key = getUniqueKey();
        UINT32 size = 1024 * (i + 1); // Different sizes
        INT32 errCode = 0;
        
        void* shmPtr = SGSYS_NewShm(key, size, &errCode);
        
        EXPECT_NE(shmPtr, nullptr);
        EXPECT_EQ(errCode, SDSHM_SUCCESS);
        
        keys.push_back(key);
        pointers.push_back(shmPtr);
        
        // Write unique data to each segment
        if (shmPtr != nullptr) {
            int* data = static_cast<int*>(shmPtr);
            *data = i * 100;
        }
    }
    
    // Verify all segments have correct data
    for (int i = 0; i < numSegments; ++i) {
        if (pointers[i] != nullptr) {
            int* data = static_cast<int*>(pointers[i]);
            EXPECT_EQ(*data, i * 100);
        }
    }
    
    // Clean up all segments
    for (INT32 key : keys) {
        SGSYS_DeleteShm(key);
    }
}

TEST_F(SDShmTest, DeleteNonExistentKey) {
    INT32 nonExistentKey = 99999;
    
    // Try to delete non-existent shared memory
    INT32 result = SGSYS_DeleteShm(nonExistentKey);
    EXPECT_EQ(result, -1);
}

TEST_F(SDShmTest, DataIntegrityAcrossOperations) {
    INT32 key = getUniqueKey();
    UINT32 size = 4096;
    INT32 errCode = 0;
    
    // Create shared memory
    void* shmPtr = SGSYS_NewShm(key, size, &errCode);
    EXPECT_NE(shmPtr, nullptr);
    EXPECT_EQ(errCode, SDSHM_SUCCESS);
    
    if (shmPtr != nullptr) {
        // Fill with pattern
        unsigned char* data = static_cast<unsigned char*>(shmPtr);
        for (UINT32 i = 0; i < size; ++i) {
            data[i] = i % 256;
        }
        
        // Verify pattern immediately
        for (UINT32 i = 0; i < size; ++i) {
            EXPECT_EQ(data[i], i % 256);
        }
        
        // Simulate some operations
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        
        // Verify pattern is still intact
        for (UINT32 i = 0; i < size; ++i) {
            EXPECT_EQ(data[i], i % 256);
        }
        
        // Modify pattern
        for (UINT32 i = 0; i < size; ++i) {
            data[i] = (255 - (i % 256));
        }
        
        // Verify new pattern
        for (UINT32 i = 0; i < size; ++i) {
            EXPECT_EQ(data[i], (255 - (i % 256)));
        }
    }
    
    // Clean up
    SGSYS_DeleteShm(key);
}

TEST_F(SDShmTest, BoundaryConditions) {
    // Test minimum size
    {
        INT32 key = getUniqueKey();
        INT32 errCode = 0;
        void* shmPtr = SGSYS_NewShm(key, 1, &errCode);
        
        if (shmPtr != nullptr) {
            EXPECT_EQ(errCode, SDSHM_SUCCESS);
            
            // Write and read single byte
            char* data = static_cast<char*>(shmPtr);
            *data = 'A';
            EXPECT_EQ(*data, 'A');
            
            SGSYS_DeleteShm(key);
        }
    }
    
    // Test page-aligned sizes
    {
        INT32 key = getUniqueKey();
        UINT32 pageSize = 4096;
        INT32 errCode = 0;
        
        void* shmPtr = SGSYS_NewShm(key, pageSize, &errCode);
        EXPECT_NE(shmPtr, nullptr);
        EXPECT_EQ(errCode, SDSHM_SUCCESS);
        
        if (shmPtr != nullptr) {
            // Write to first and last bytes
            char* data = static_cast<char*>(shmPtr);
            data[0] = 'F';
            data[pageSize - 1] = 'L';
            
            EXPECT_EQ(data[0], 'F');
            EXPECT_EQ(data[pageSize - 1], 'L');
            
            SGSYS_DeleteShm(key);
        }
    }
}

TEST_F(SDShmTest, ErrorCodeVerification) {
    std::vector<std::pair<INT32, INT32>> testCases;
    
    // Test different error conditions
    INT32 key1 = getUniqueKey();
    INT32 errCode = 0;
    
    // Create valid shared memory first
    void* validPtr = SGSYS_NewShm(key1, 1024, &errCode);
    EXPECT_EQ(errCode, SDSHM_SUCCESS);
    
    // Test duplicate key
    void* dupPtr = SGSYS_NewShm(key1, 1024, &errCode);
    EXPECT_EQ(errCode, SDSHM_ALREADYINUSE);
    EXPECT_EQ(dupPtr, nullptr);
    
    // Test invalid size
    INT32 key2 = getUniqueKey();
    void* invalidPtr = SGSYS_NewShm(key2, 0, &errCode);
    EXPECT_EQ(errCode, SDSHM_INVALIDPARAM);
    EXPECT_EQ(invalidPtr, nullptr);
    
    // Clean up
    if (validPtr != nullptr) {
        SGSYS_DeleteShm(key1);
    }
}

// Performance test
TEST_F(SDShmTest, PerformanceTest) {
    const int numOperations = 100;
    std::vector<INT32> keys;
    std::vector<void*> pointers;
    
    // Measure creation time
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numOperations; ++i) {
        INT32 key = getUniqueKey();
        INT32 errCode = 0;
        void* ptr = SGSYS_NewShm(key, 4096, &errCode);
        
        if (ptr != nullptr) {
            keys.push_back(key);
            pointers.push_back(ptr);
            
            // Write some data
            int* data = static_cast<int*>(ptr);
            *data = i;
        }
    }
    
    auto createTime = std::chrono::high_resolution_clock::now() - start;
    
    // Measure access time
    start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < pointers.size(); ++i) {
        if (pointers[i] != nullptr) {
            int* data = static_cast<int*>(pointers[i]);
            volatile int value = *data; // Volatile to prevent optimization
            (void)value; // Suppress unused variable warning
        }
    }
    
    auto accessTime = std::chrono::high_resolution_clock::now() - start;
    
    // Measure deletion time
    start = std::chrono::high_resolution_clock::now();
    
    for (INT32 key : keys) {
        SGSYS_DeleteShm(key);
    }
    
    auto deleteTime = std::chrono::high_resolution_clock::now() - start;
    
    auto createMicros = std::chrono::duration_cast<std::chrono::microseconds>(createTime).count();
    auto accessMicros = std::chrono::duration_cast<std::chrono::microseconds>(accessTime).count();
    auto deleteMicros = std::chrono::duration_cast<std::chrono::microseconds>(deleteTime).count();
    
    std::cout << "Shared memory performance for " << numOperations << " operations:\n";
    std::cout << "Creation time: " << createMicros << " μs\n";
    std::cout << "Access time: " << accessMicros << " μs\n";
    std::cout << "Deletion time: " << deleteMicros << " μs\n";
    
    // Performance should be reasonable
    EXPECT_LT(createMicros, 10000000); // Less than 10 seconds
    EXPECT_LT(accessMicros, 1000000);  // Less than 1 second
    EXPECT_LT(deleteMicros, 5000000);  // Less than 5 seconds
}