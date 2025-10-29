/**
 * @file test_sdindexer.cpp
 * @brief Indexer functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdindexer.h"
#include <string>
#include <vector>
#include <set>

using namespace SSCP;

class SDIndexerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// Test data structures
struct TestData {
    int value;
    std::string name;
    
    TestData(int v = 0, const std::string& n = "") : value(v), name(n) {}
    
    bool operator==(const TestData& other) const {
        return value == other.value && name == other.name;
    }
};

// Custom equality comparator for testing
struct TestDataEqual {
    bool operator()(const TestData& left, const TestData& right) const {
        return left.value == right.value && left.name == right.name;
    }
};

// Basic functionality tests
TEST_F(SDIndexerTest, BasicAllocation) {
    SDIndexer<int, 10> indexer;
    
    // Test basic allocation
    int value1 = 42;
    UINT32 id1 = indexer.Alloc(value1);
    
    EXPECT_NE(id1, SDINVALID_INDEX);
    EXPECT_LT(id1, 10); // Should be within bounds
    EXPECT_TRUE(indexer.Exist(id1));
    
    // Verify the stored value
    int retrieved = indexer.Get(id1);
    EXPECT_EQ(retrieved, 42);
}

TEST_F(SDIndexerTest, MultipleAllocations) {
    SDIndexer<int, 5> indexer;
    
    std::vector<UINT32> ids;
    
    // Allocate all available slots
    for (int i = 0; i < 4; ++i) { // maxSize - 1 (index 0 is invalid)
        int value = i * 10;
        UINT32 id = indexer.Alloc(value);
        EXPECT_NE(id, SDINVALID_INDEX);
        ids.push_back(id);
        
        EXPECT_TRUE(indexer.Exist(id));
        EXPECT_EQ(indexer.Get(id), value);
    }
    
    // Try to allocate when full - should fail
    int extraValue = 999;
    UINT32 invalidId = indexer.Alloc(extraValue);
    EXPECT_EQ(invalidId, SDINVALID_INDEX);
    
    // Verify all allocated values are still correct
    for (size_t i = 0; i < ids.size(); ++i) {
        EXPECT_EQ(indexer.Get(ids[i]), (int)(i * 10));
    }
}

TEST_F(SDIndexerTest, FindOperation) {
    SDIndexer<int, 10> indexer;
    
    // Allocate some values
    int value1 = 100;
    int value2 = 200;
    int value3 = 300;
    
    UINT32 id1 = indexer.Alloc(value1);
    UINT32 id2 = indexer.Alloc(value2);
    UINT32 id3 = indexer.Alloc(value3);
    
    // Test finding existing values
    EXPECT_EQ(indexer.Find(100), id1);
    EXPECT_EQ(indexer.Find(200), id2);
    EXPECT_EQ(indexer.Find(300), id3);
    
    // Test finding non-existent value
    EXPECT_EQ(indexer.Find(999), SDINVALID_INDEX);
}

TEST_F(SDIndexerTest, FreeAndReuse) {
    SDIndexer<int, 5> indexer;
    
    // Allocate a value
    int value1 = 42;
    UINT32 id1 = indexer.Alloc(value1);
    EXPECT_NE(id1, SDINVALID_INDEX);
    EXPECT_TRUE(indexer.Exist(id1));
    
    // Free the value
    int freedValue = indexer.Free(id1);
    EXPECT_EQ(freedValue, 42);
    EXPECT_FALSE(indexer.Exist(id1));
    
    // Find should no longer work
    EXPECT_EQ(indexer.Find(42), SDINVALID_INDEX);
    
    // The ID should be reusable
    int value2 = 84;
    UINT32 id2 = indexer.Alloc(value2);
    EXPECT_NE(id2, SDINVALID_INDEX);
    EXPECT_TRUE(indexer.Exist(id2));
    
    // The new value should be findable
    EXPECT_EQ(indexer.Find(84), id2);
}

TEST_F(SDIndexerTest, InvalidOperations) {
    SDIndexer<int, 5> indexer;
    
    // Test operations with invalid IDs
    EXPECT_FALSE(indexer.Exist(SDINVALID_INDEX));
    EXPECT_FALSE(indexer.Exist(999)); // Out of bounds
    
    // Get with invalid ID should return default value
    int defaultValue = indexer.Get(SDINVALID_INDEX);
    EXPECT_EQ(defaultValue, 0); // Default for int
    
    int outOfBounds = indexer.Get(999);
    EXPECT_EQ(outOfBounds, 0);
    
    // Free with invalid ID should return default value
    int freedInvalid = indexer.Free(SDINVALID_INDEX);
    EXPECT_EQ(freedInvalid, 0);
    
    int freedOutOfBounds = indexer.Free(999);
    EXPECT_EQ(freedOutOfBounds, 0);
}

TEST_F(SDIndexerTest, CustomDataType) {
    SDIndexer<TestData, 10, TestDataEqual> indexer;
    
    // Allocate custom objects
    TestData data1(1, "first");
    TestData data2(2, "second");
    
    UINT32 id1 = indexer.Alloc(data1);
    UINT32 id2 = indexer.Alloc(data2);
    
    EXPECT_NE(id1, SDINVALID_INDEX);
    EXPECT_NE(id2, SDINVALID_INDEX);
    EXPECT_NE(id1, id2);
    
    // Verify stored data
    TestData retrieved1 = indexer.Get(id1);
    TestData retrieved2 = indexer.Get(id2);
    
    EXPECT_EQ(retrieved1.value, 1);
    EXPECT_EQ(retrieved1.name, "first");
    EXPECT_EQ(retrieved2.value, 2);
    EXPECT_EQ(retrieved2.name, "second");
    
    // Test find with custom equality
    EXPECT_EQ(indexer.Find(TestData(1, "first")), id1);
    EXPECT_EQ(indexer.Find(TestData(2, "second")), id2);
    EXPECT_EQ(indexer.Find(TestData(3, "third")), SDINVALID_INDEX);
}

TEST_F(SDIndexerTest, LargeCapacity) {
    SDIndexer<int, 1000> indexer;
    
    std::vector<UINT32> ids;
    std::set<UINT32> uniqueIds;
    
    // Allocate many values
    for (int i = 0; i < 500; ++i) {
        UINT32 id = indexer.Alloc(i);
        EXPECT_NE(id, SDINVALID_INDEX);
        ids.push_back(id);
        uniqueIds.insert(id);
        
        EXPECT_TRUE(indexer.Exist(id));
        EXPECT_EQ(indexer.Get(id), i);
    }
    
    // All IDs should be unique
    EXPECT_EQ(uniqueIds.size(), 500);
    
    // Verify all values are still correct
    for (size_t i = 0; i < ids.size(); ++i) {
        EXPECT_EQ(indexer.Get(ids[i]), (int)i);
        EXPECT_EQ(indexer.Find((int)i), ids[i]);
    }
    
    // Free some values
    for (size_t i = 0; i < 100; ++i) {
        int freedValue = indexer.Free(ids[i]);
        EXPECT_EQ(freedValue, (int)i);
        EXPECT_FALSE(indexer.Exist(ids[i]));
    }
    
    // Reallocate - should get some reused IDs
    for (int i = 500; i < 600; ++i) {
        UINT32 id = indexer.Alloc(i);
        EXPECT_NE(id, SDINVALID_INDEX);
        EXPECT_TRUE(indexer.Exist(id));
        EXPECT_EQ(indexer.Get(id), i);
    }
}

TEST_F(SDIndexerTest, PointerTypes) {
    SDIndexer<int*, 10> indexer;
    
    int value1 = 42;
    int value2 = 84;
    
    // Allocate pointers
    int* ptr1 = &value1;
    int* ptr2 = &value2;
    UINT32 id1 = indexer.Alloc(ptr1);
    UINT32 id2 = indexer.Alloc(ptr2);
    
    EXPECT_NE(id1, SDINVALID_INDEX);
    EXPECT_NE(id2, SDINVALID_INDEX);
    
    // Verify pointer values
    int* retrievedPtr1 = indexer.Get(id1);
    int* retrievedPtr2 = indexer.Get(id2);
    
    EXPECT_EQ(retrievedPtr1, ptr1);
    EXPECT_EQ(retrievedPtr2, ptr2);
    EXPECT_EQ(*retrievedPtr1, 42);
    EXPECT_EQ(*retrievedPtr2, 84);
    
    // Test find with pointers
    EXPECT_EQ(indexer.Find(&value1), id1);
    EXPECT_EQ(indexer.Find(&value2), id2);
    
    // Test with null pointer
    int* nullPtr = nullptr;
    UINT32 id3 = indexer.Alloc(nullPtr);
    EXPECT_NE(id3, SDINVALID_INDEX);
    EXPECT_EQ(indexer.Get(id3), nullptr);
}

TEST_F(SDIndexerTest, StringType) {
    SDIndexer<std::string, 20> indexer;
    
    std::string str1 = "Hello";
    std::string str2 = "World";
    std::string str3 = "Test String";
    
    UINT32 id1 = indexer.Alloc(str1);
    UINT32 id2 = indexer.Alloc(str2);
    UINT32 id3 = indexer.Alloc(str3);
    
    EXPECT_NE(id1, SDINVALID_INDEX);
    EXPECT_NE(id2, SDINVALID_INDEX);
    EXPECT_NE(id3, SDINVALID_INDEX);
    
    // Verify string values
    EXPECT_EQ(indexer.Get(id1), "Hello");
    EXPECT_EQ(indexer.Get(id2), "World");
    EXPECT_EQ(indexer.Get(id3), "Test String");
    
    // Test find with strings
    EXPECT_EQ(indexer.Find(std::string("Hello")), id1);
    EXPECT_EQ(indexer.Find(std::string("World")), id2);
    EXPECT_EQ(indexer.Find(std::string("Test String")), id3);
    EXPECT_EQ(indexer.Find(std::string("Not Found")), SDINVALID_INDEX);
}

TEST_F(SDIndexerTest, AllocationPattern) {
    SDIndexer<int, 8> indexer;
    
    std::vector<UINT32> ids;
    
    // Fill the indexer
    for (int i = 0; i < 7; ++i) { // 7 = maxSize - 1
        int value = i;
        UINT32 id = indexer.Alloc(value);
        EXPECT_NE(id, SDINVALID_INDEX);
        ids.push_back(id);
    }
    
    // Should be full now
    int failValue = 999;
    UINT32 failId = indexer.Alloc(failValue);
    EXPECT_EQ(failId, SDINVALID_INDEX);
    
    // Free some in the middle
    indexer.Free(ids[2]);
    indexer.Free(ids[4]);
    
    // Should be able to allocate again
    int newValue1 = 100;
    int newValue2 = 200;
    UINT32 newId1 = indexer.Alloc(newValue1);
    UINT32 newId2 = indexer.Alloc(newValue2);
    
    EXPECT_NE(newId1, SDINVALID_INDEX);
    EXPECT_NE(newId2, SDINVALID_INDEX);
    EXPECT_EQ(indexer.Get(newId1), 100);
    EXPECT_EQ(indexer.Get(newId2), 200);
    
    // Should be full again
    int anotherFailValue = 999;
    failId = indexer.Alloc(anotherFailValue);
    EXPECT_EQ(failId, SDINVALID_INDEX);
}

// Edge case tests
TEST_F(SDIndexerTest, MinimalCapacity) {
    SDIndexer<int, 2> indexer; // Only 1 usable slot (index 0 is invalid)
    
    // Should be able to allocate one
    int value1 = 42;
    UINT32 id1 = indexer.Alloc(value1);
    EXPECT_NE(id1, SDINVALID_INDEX);
    EXPECT_EQ(indexer.Get(id1), 42);
    
    // Should fail to allocate second
    int value2 = 84;
    UINT32 id2 = indexer.Alloc(value2);
    EXPECT_EQ(id2, SDINVALID_INDEX);
    
    // Free and should be able to allocate again
    indexer.Free(id1);
    id2 = indexer.Alloc(84);
    EXPECT_NE(id2, SDINVALID_INDEX);
    EXPECT_EQ(indexer.Get(id2), 84);
}

TEST_F(SDIndexerTest, ZeroValue) {
    SDIndexer<int, 10> indexer;
    
    // Allocate zero value
    int zero = 0;
    UINT32 id = indexer.Alloc(zero);
    EXPECT_NE(id, SDINVALID_INDEX);
    EXPECT_EQ(indexer.Get(id), 0);
    EXPECT_EQ(indexer.Find(0), id);
}

// Performance test
TEST_F(SDIndexerTest, PerformanceTest) {
    SDIndexer<int, 10000> indexer;
    
    std::vector<UINT32> ids;
    ids.reserve(5000);
    
    // Measure allocation performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 5000; ++i) {
        UINT32 id = indexer.Alloc(i);
        EXPECT_NE(id, SDINVALID_INDEX);
        ids.push_back(id);
    }
    
    auto allocTime = std::chrono::high_resolution_clock::now() - start;
    
    // Measure find performance
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 5000; ++i) {
        UINT32 foundId = indexer.Find(i);
        EXPECT_EQ(foundId, ids[i]);
    }
    
    auto findTime = std::chrono::high_resolution_clock::now() - start;
    
    // Measure free performance
    start = std::chrono::high_resolution_clock::now();
    
    for (UINT32 id : ids) {
        indexer.Free(id);
    }
    
    auto freeTime = std::chrono::high_resolution_clock::now() - start;
    
    // Print performance results
    auto allocMicros = std::chrono::duration_cast<std::chrono::microseconds>(allocTime).count();
    auto findMicros = std::chrono::duration_cast<std::chrono::microseconds>(findTime).count();
    auto freeMicros = std::chrono::duration_cast<std::chrono::microseconds>(freeTime).count();
    
    std::cout << "Allocation time for 5000 items: " << allocMicros << " μs\n";
    std::cout << "Find time for 5000 items: " << findMicros << " μs\n";
    std::cout << "Free time for 5000 items: " << freeMicros << " μs\n";
    
    // Performance should be reasonable
    EXPECT_LT(allocMicros, 100000); // Less than 0.1 second
    EXPECT_LT(findMicros, 1000000);  // Less than 1 second (find is O(n))
    EXPECT_LT(freeMicros, 100000);   // Less than 0.1 second
}