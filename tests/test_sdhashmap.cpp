/**
 * @file test_sdhashmap.cpp
 * @brief Hash map functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sdhashmap.h"
#include <string>
#include <vector>

class SDHashMapTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test setup
    }

    void TearDown() override {
        // Test cleanup
    }
};

// Basic HashMap functionality tests
TEST_F(SDHashMapTest, BasicInsertAndFind) {
    HashMap<int, std::string> map;
    
    // Test insertion
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";
    
    // Test size
    EXPECT_EQ(map.size(), 3);
    EXPECT_FALSE(map.empty());
    
    // Test find
    auto it1 = map.find(1);
    EXPECT_NE(it1, map.end());
    EXPECT_EQ(it1->second, "one");
    
    auto it2 = map.find(2);
    EXPECT_NE(it2, map.end());
    EXPECT_EQ(it2->second, "two");
    
    auto it3 = map.find(3);
    EXPECT_NE(it3, map.end());
    EXPECT_EQ(it3->second, "three");
    
    // Test find non-existent
    auto it4 = map.find(4);
    EXPECT_EQ(it4, map.end());
}

TEST_F(SDHashMapTest, StringKeys) {
    HashMap<std::string, int> map;
    
    // Test string keys
    map["apple"] = 1;
    map["banana"] = 2;
    map["cherry"] = 3;
    
    EXPECT_EQ(map.size(), 3);
    
    // Test access
    EXPECT_EQ(map["apple"], 1);
    EXPECT_EQ(map["banana"], 2);
    EXPECT_EQ(map["cherry"], 3);
    
    // Test find with string keys
    auto it = map.find("banana");
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->second, 2);
    
    // Test non-existent key
    auto it2 = map.find("orange");
    EXPECT_EQ(it2, map.end());
}

TEST_F(SDHashMapTest, UpdateValues) {
    HashMap<int, std::string> map;
    
    // Insert initial values
    map[1] = "original";
    map[2] = "value";
    
    EXPECT_EQ(map[1], "original");
    EXPECT_EQ(map[2], "value");
    
    // Update values
    map[1] = "updated";
    map[2] = "modified";
    
    EXPECT_EQ(map[1], "updated");
    EXPECT_EQ(map[2], "modified");
    EXPECT_EQ(map.size(), 2); // Size should remain the same
}

TEST_F(SDHashMapTest, EraseElements) {
    HashMap<int, std::string> map;
    
    // Insert elements
    for (int i = 1; i <= 5; ++i) {
        map[i] = "value" + std::to_string(i);
    }
    
    EXPECT_EQ(map.size(), 5);
    
    // Erase by key
    auto erased = map.erase(3);
    EXPECT_EQ(erased, 1); // Should return number of erased elements
    EXPECT_EQ(map.size(), 4);
    EXPECT_EQ(map.find(3), map.end());
    
    // Erase by iterator
    auto it = map.find(2);
    EXPECT_NE(it, map.end());
    map.erase(it);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.find(2), map.end());
    
    // Erase non-existent key
    auto erased2 = map.erase(10);
    EXPECT_EQ(erased2, 0);
    EXPECT_EQ(map.size(), 3);
}

TEST_F(SDHashMapTest, ClearAndEmpty) {
    HashMap<int, std::string> map;
    
    // Test empty map
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    
    // Add elements
    map[1] = "one";
    map[2] = "two";
    
    EXPECT_FALSE(map.empty());
    EXPECT_EQ(map.size(), 2);
    
    // Clear map
    map.clear();
    
    EXPECT_TRUE(map.empty());
    EXPECT_EQ(map.size(), 0);
    EXPECT_EQ(map.find(1), map.end());
    EXPECT_EQ(map.find(2), map.end());
}

TEST_F(SDHashMapTest, IteratorTraversal) {
    HashMap<int, std::string> map;
    
    // Insert elements
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";
    map[4] = "four";
    map[5] = "five";
    
    // Test iterator traversal
    std::vector<std::pair<int, std::string>> elements;
    for (auto it = map.begin(); it != map.end(); ++it) {
        elements.push_back(*it);
    }
    
    EXPECT_EQ(elements.size(), 5);
    
    // Verify all elements are present (order may vary)
    std::vector<int> keys;
    std::vector<std::string> values;
    for (const auto& elem : elements) {
        keys.push_back(elem.first);
        values.push_back(elem.second);
    }
    
    std::sort(keys.begin(), keys.end());
    std::sort(values.begin(), values.end());
    
    EXPECT_EQ(keys, std::vector<int>({1, 2, 3, 4, 5}));
    EXPECT_EQ(values, std::vector<std::string>({"five", "four", "one", "three", "two"}));
}

TEST_F(SDHashMapTest, RangeBasedLoop) {
    HashMap<std::string, int> map;
    
    map["a"] = 1;
    map["b"] = 2;
    map["c"] = 3;
    
    // Test range-based for loop
    int sum = 0;
    std::vector<std::string> keys;
    
    for (const auto& pair : map) {
        sum += pair.second;
        keys.push_back(pair.first);
    }
    
    EXPECT_EQ(sum, 6);
    EXPECT_EQ(keys.size(), 3);
    
    // Sort keys to make test deterministic
    std::sort(keys.begin(), keys.end());
    EXPECT_EQ(keys, std::vector<std::string>({"a", "b", "c"}));
}

TEST_F(SDHashMapTest, CountAndContains) {
    HashMap<int, std::string> map;
    
    map[1] = "one";
    map[2] = "two";
    map[3] = "three";
    
    // Test count (should be 0 or 1 for unordered_map)
    EXPECT_EQ(map.count(1), 1);
    EXPECT_EQ(map.count(2), 1);
    EXPECT_EQ(map.count(3), 1);
    EXPECT_EQ(map.count(4), 0);
    EXPECT_EQ(map.count(10), 0);
}

TEST_F(SDHashMapTest, InsertMethod) {
    HashMap<int, std::string> map;
    
    // Test insert method
    auto result1 = map.insert({1, "one"});
    EXPECT_TRUE(result1.second); // Should be inserted
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    
    // Try to insert duplicate key
    auto result2 = map.insert({1, "uno"});
    EXPECT_FALSE(result2.second); // Should not be inserted
    EXPECT_EQ(result2.first->first, 1);
    EXPECT_EQ(result2.first->second, "one"); // Original value preserved
    
    // Insert another element
    auto result3 = map.insert({2, "two"});
    EXPECT_TRUE(result3.second);
    EXPECT_EQ(map.size(), 2);
}

TEST_F(SDHashMapTest, EmplaceMethod) {
    HashMap<int, std::string> map;
    
    // Test emplace
    auto result1 = map.emplace(1, "one");
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(result1.first->first, 1);
    EXPECT_EQ(result1.first->second, "one");
    
    // Try to emplace duplicate
    auto result2 = map.emplace(1, "uno");
    EXPECT_FALSE(result2.second);
    EXPECT_EQ(result2.first->second, "one");
    
    EXPECT_EQ(map.size(), 1);
}

TEST_F(SDHashMapTest, LargeDataSet) {
    HashMap<int, int> map;
    
    const int numElements = 10000;
    
    // Insert large number of elements
    for (int i = 0; i < numElements; ++i) {
        map[i] = i * 2;
    }
    
    EXPECT_EQ(map.size(), numElements);
    
    // Verify all elements
    for (int i = 0; i < numElements; ++i) {
        auto it = map.find(i);
        EXPECT_NE(it, map.end());
        EXPECT_EQ(it->second, i * 2);
    }
    
    // Remove half of the elements
    for (int i = 0; i < numElements; i += 2) {
        map.erase(i);
    }
    
    EXPECT_EQ(map.size(), numElements / 2);
    
    // Verify remaining elements
    for (int i = 1; i < numElements; i += 2) {
        auto it = map.find(i);
        EXPECT_NE(it, map.end());
        EXPECT_EQ(it->second, i * 2);
    }
}

TEST_F(SDHashMapTest, CustomHashableType) {
    struct Point {
        int x, y;
        
        Point(int x = 0, int y = 0) : x(x), y(y) {}
        
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };
    
    // Custom hash function for Point
    struct PointHash {
        std::size_t operator()(const Point& p) const {
            return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
        }
    };
    
    std::unordered_map<Point, std::string, PointHash> map;
    
    map[Point(1, 2)] = "point1";
    map[Point(3, 4)] = "point2";
    map[Point(5, 6)] = "point3";
    
    EXPECT_EQ(map.size(), 3);
    
    auto it = map.find(Point(3, 4));
    EXPECT_NE(it, map.end());
    EXPECT_EQ(it->second, "point2");
}

// HashSet tests
TEST_F(SDHashMapTest, HashSetBasicOperations) {
    HashSet<int> set;
    
    // Test insertion
    auto result1 = set.insert(1);
    EXPECT_TRUE(result1.second);
    EXPECT_EQ(*result1.first, 1);
    
    auto result2 = set.insert(2);
    EXPECT_TRUE(result2.second);
    
    auto result3 = set.insert(3);
    EXPECT_TRUE(result3.second);
    
    EXPECT_EQ(set.size(), 3);
    EXPECT_FALSE(set.empty());
    
    // Test duplicate insertion
    auto result4 = set.insert(2);
    EXPECT_FALSE(result4.second);
    EXPECT_EQ(set.size(), 3);
}

TEST_F(SDHashMapTest, HashSetFindAndCount) {
    HashSet<std::string> set;
    
    set.insert("apple");
    set.insert("banana");
    set.insert("cherry");
    
    // Test find
    auto it1 = set.find("banana");
    EXPECT_NE(it1, set.end());
    EXPECT_EQ(*it1, "banana");
    
    auto it2 = set.find("orange");
    EXPECT_EQ(it2, set.end());
    
    // Test count
    EXPECT_EQ(set.count("apple"), 1);
    EXPECT_EQ(set.count("grape"), 0);
}

TEST_F(SDHashMapTest, HashSetEraseAndClear) {
    HashSet<int> set;
    
    // Insert elements
    for (int i = 1; i <= 5; ++i) {
        set.insert(i);
    }
    
    EXPECT_EQ(set.size(), 5);
    
    // Erase by value
    auto erased = set.erase(3);
    EXPECT_EQ(erased, 1);
    EXPECT_EQ(set.size(), 4);
    EXPECT_EQ(set.find(3), set.end());
    
    // Erase by iterator
    auto it = set.find(2);
    EXPECT_NE(it, set.end());
    set.erase(it);
    EXPECT_EQ(set.size(), 3);
    EXPECT_EQ(set.find(2), set.end());
    
    // Clear set
    set.clear();
    EXPECT_TRUE(set.empty());
    EXPECT_EQ(set.size(), 0);
}

TEST_F(SDHashMapTest, HashSetIteration) {
    HashSet<int> set;
    
    // Insert elements
    std::vector<int> values = {5, 2, 8, 1, 9, 3};
    for (int val : values) {
        set.insert(val);
    }
    
    EXPECT_EQ(set.size(), 6);
    
    // Collect all elements via iteration
    std::vector<int> collected;
    for (auto it = set.begin(); it != set.end(); ++it) {
        collected.push_back(*it);
    }
    
    EXPECT_EQ(collected.size(), 6);
    
    // Sort for comparison (hash set doesn't guarantee order)
    std::sort(collected.begin(), collected.end());
    std::sort(values.begin(), values.end());
    EXPECT_EQ(collected, values);
}

// Performance test
TEST_F(SDHashMapTest, PerformanceTest) {
    HashMap<int, int> map;
    HashSet<int> set;
    
    const int numElements = 50000;
    
    // Measure insertion performance
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numElements; ++i) {
        map[i] = i * 2;
        set.insert(i);
    }
    
    auto insertTime = std::chrono::high_resolution_clock::now() - start;
    
    // Measure lookup performance
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < numElements; ++i) {
        auto mapIt = map.find(i);
        auto setIt = set.find(i);
        EXPECT_NE(mapIt, map.end());
        EXPECT_NE(setIt, set.end());
    }
    
    auto lookupTime = std::chrono::high_resolution_clock::now() - start;
    
    auto insertMicros = std::chrono::duration_cast<std::chrono::microseconds>(insertTime).count();
    auto lookupMicros = std::chrono::duration_cast<std::chrono::microseconds>(lookupTime).count();
    
    std::cout << "HashMap/HashSet insertion time for " << numElements << " elements: " << insertMicros << " μs\n";
    std::cout << "HashMap/HashSet lookup time for " << numElements << " elements: " << lookupMicros << " μs\n";
    
    // Performance should be reasonable
    EXPECT_LT(insertMicros, 1000000); // Less than 1 second
    EXPECT_LT(lookupMicros, 500000);  // Less than 0.5 second
}