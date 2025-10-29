/**
 * @file test_sddir.cpp
 * @brief Directory utilities functionality tests
 */

#include <gtest/gtest.h>
#include "ssengine/sddir.h"
#include "ssengine/sdfile.h"
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>

using namespace SSCP;

class SDDirTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory structure
        testRoot = "test_dir_" + std::to_string(std::time(nullptr));
        
        // Create main test directory
        ASSERT_TRUE(SDCreateDirectory(testRoot.c_str()));
        
        // Create subdirectories
        std::string subDir1 = testRoot + "/sub1";
        std::string subDir2 = testRoot + "/sub2";
        std::string emptyDir = testRoot + "/empty";
        
        ASSERT_TRUE(SDCreateDirectory(subDir1.c_str()));
        ASSERT_TRUE(SDCreateDirectory(subDir2.c_str()));
        ASSERT_TRUE(SDCreateDirectory(emptyDir.c_str()));
        
        // Create test files
        createTestFile(testRoot + "/file1.txt", "Content of file1");
        createTestFile(testRoot + "/file2.log", "Content of file2");
        createTestFile(subDir1 + "/nested_file.txt", "Nested file content");
        
        createdPaths.push_back(testRoot);
    }

    void TearDown() override {
        // Clean up test directories and files
        for (const auto& path : createdPaths) {
            SDDeleteDirectory(path.c_str(), TRUE); // Force delete
        }
    }
    
    void createTestFile(const std::string& path, const std::string& content) {
        std::ofstream file(path);
        if (file.is_open()) {
            file << content;
            file.close();
        }
    }
    
    std::string testRoot;
    std::vector<std::string> createdPaths;
};

// Basic directory operations tests
TEST_F(SDDirTest, CreateAndDeleteDirectory) {
    std::string newDir = testRoot + "/new_directory";
    
    // Directory should not exist initially
    EXPECT_FALSE(SDDirectoryExists(newDir.c_str()));
    
    // Create directory
    EXPECT_TRUE(SDCreateDirectory(newDir.c_str()));
    EXPECT_TRUE(SDDirectoryExists(newDir.c_str()));
    EXPECT_TRUE(SDIsDirectory(newDir.c_str()));
    
    // Delete directory
    EXPECT_TRUE(SDDeleteDirectory(newDir.c_str()));
    EXPECT_FALSE(SDDirectoryExists(newDir.c_str()));
}

TEST_F(SDDirTest, CreateDirectoryWithForce) {
    std::string deepPath = testRoot + "/level1/level2/level3";
    
    // Should fail without force (parent directories don't exist)
    EXPECT_FALSE(SDCreateDirectory(deepPath.c_str(), FALSE));
    
    // Should succeed with force
    EXPECT_TRUE(SDCreateDirectory(deepPath.c_str(), TRUE));
    EXPECT_TRUE(SDDirectoryExists(deepPath.c_str()));
    
    // Verify intermediate directories were created
    EXPECT_TRUE(SDDirectoryExists((testRoot + "/level1").c_str()));
    EXPECT_TRUE(SDDirectoryExists((testRoot + "/level1/level2").c_str()));
}

TEST_F(SDDirTest, DirectoryExistsAndIsDirectory) {
    // Test existing directory
    EXPECT_TRUE(SDDirectoryExists(testRoot.c_str()));
    EXPECT_TRUE(SDIsDirectory(testRoot.c_str()));
    
    // Test existing file (should not be directory)
    std::string filePath = testRoot + "/file1.txt";
    EXPECT_FALSE(SDIsDirectory(filePath.c_str()));
    
    // Test non-existent path
    EXPECT_FALSE(SDDirectoryExists("non_existent_directory"));
    EXPECT_FALSE(SDIsDirectory("non_existent_directory"));
    
    // Test null pointer
    EXPECT_FALSE(SDDirectoryExists(nullptr));
    EXPECT_FALSE(SDIsDirectory(nullptr));
}

TEST_F(SDDirTest, IsEmptyDirectory) {
    // Test empty directory
    std::string emptyDir = testRoot + "/empty";
    EXPECT_TRUE(SDIsEmptyDirectory(emptyDir.c_str()));
    
    // Test non-empty directory
    EXPECT_FALSE(SDIsEmptyDirectory(testRoot.c_str()));
    
    // Test with file (should return FALSE)
    std::string filePath = testRoot + "/file1.txt";
    EXPECT_FALSE(SDIsEmptyDirectory(filePath.c_str()));
    
    // Test non-existent directory
    EXPECT_FALSE(SDIsEmptyDirectory("non_existent"));
    
    // Create file in empty directory and test again
    createTestFile(emptyDir + "/new_file.txt", "content");
    EXPECT_FALSE(SDIsEmptyDirectory(emptyDir.c_str()));
}

// CSDDirectory class tests
TEST_F(SDDirTest, DirectoryClassBasicOperations) {
    CSDDirectory dir;
    
    // Open directory
    EXPECT_TRUE(dir.Open(testRoot.c_str()));
    
    std::vector<std::string> entries;
    std::vector<BOOL> isDirectoryFlags;
    
    // Read all entries
    char name[256];
    SFileAttr attr;
    while (dir.Read(name, sizeof(name), &attr)) {
        entries.push_back(std::string(name));
        isDirectoryFlags.push_back(attr.isDir);
    }
    
    dir.Close();
    
    // Should have found our test files and directories
    EXPECT_GE(entries.size(), 5); // At least 5 entries (3 dirs + 2 files)
    
    // Verify specific entries exist
    bool foundFile1 = false, foundFile2 = false;
    bool foundSub1 = false, foundSub2 = false, foundEmpty = false;
    
    for (size_t i = 0; i < entries.size(); ++i) {
        if (entries[i] == "file1.txt") {
            foundFile1 = true;
            EXPECT_FALSE(isDirectoryFlags[i]);
        } else if (entries[i] == "file2.log") {
            foundFile2 = true;
            EXPECT_FALSE(isDirectoryFlags[i]);
        } else if (entries[i] == "sub1") {
            foundSub1 = true;
            EXPECT_TRUE(isDirectoryFlags[i]);
        } else if (entries[i] == "sub2") {
            foundSub2 = true;
            EXPECT_TRUE(isDirectoryFlags[i]);
        } else if (entries[i] == "empty") {
            foundEmpty = true;
            EXPECT_TRUE(isDirectoryFlags[i]);
        }
    }
    
    EXPECT_TRUE(foundFile1);
    EXPECT_TRUE(foundFile2);
    EXPECT_TRUE(foundSub1);
    EXPECT_TRUE(foundSub2);
    EXPECT_TRUE(foundEmpty);
}

TEST_F(SDDirTest, DirectoryClassInvalidOperations) {
    CSDDirectory dir;
    
    // Test opening non-existent directory
    EXPECT_FALSE(dir.Open("non_existent_directory"));
    
    // Test opening file as directory
    std::string filePath = testRoot + "/file1.txt";
    EXPECT_FALSE(dir.Open(filePath.c_str()));
    
    // Test opening with null pointer
    EXPECT_FALSE(dir.Open(nullptr));
    
    // Test reading without opening
    char name[256];
    SFileAttr attr;
    EXPECT_FALSE(dir.Read(name, sizeof(name), &attr));
    
    // Test reading with invalid parameters
    EXPECT_TRUE(dir.Open(testRoot.c_str()));
    EXPECT_FALSE(dir.Read(nullptr, 100, &attr));
    EXPECT_FALSE(dir.Read(name, 0, &attr));
    EXPECT_FALSE(dir.Read(name, -1, &attr));
    
    dir.Close();
}

TEST_F(SDDirTest, DirectoryClassWithoutAttributes) {
    CSDDirectory dir;
    EXPECT_TRUE(dir.Open(testRoot.c_str()));
    
    // Read without attribute pointer
    char name[256];
    std::vector<std::string> entries;
    
    while (dir.Read(name, sizeof(name), nullptr)) {
        entries.push_back(std::string(name));
    }
    
    dir.Close();
    
    EXPECT_GE(entries.size(), 5);
}

TEST_F(SDDirTest, DirectoryClassSmallBuffer) {
    CSDDirectory dir;
    EXPECT_TRUE(dir.Open(testRoot.c_str()));
    
    // Use small buffer that might truncate names
    char smallName[5];
    SFileAttr attr;
    
    std::vector<std::string> truncatedEntries;
    while (dir.Read(smallName, sizeof(smallName), &attr)) {
        truncatedEntries.push_back(std::string(smallName));
        // All returned strings should be properly null-terminated
        EXPECT_LT(strlen(smallName), sizeof(smallName));
    }
    
    dir.Close();
    
    EXPECT_GT(truncatedEntries.size(), 0);
}

TEST_F(SDDirTest, NestedDirectoryOperations) {
    // Test operations on nested directories
    std::string subDir = testRoot + "/sub1";
    
    CSDDirectory dir;
    EXPECT_TRUE(dir.Open(subDir.c_str()));
    
    char name[256];
    SFileAttr attr;
    bool foundNestedFile = false;
    
    while (dir.Read(name, sizeof(name), &attr)) {
        if (std::string(name) == "nested_file.txt") {
            foundNestedFile = true;
            EXPECT_FALSE(attr.isDir);
        }
    }
    
    dir.Close();
    
    EXPECT_TRUE(foundNestedFile);
}

TEST_F(SDDirTest, DeleteDirectoryWithForce) {
    // Create a directory with content
    std::string dirWithContent = testRoot + "/dir_with_content";
    EXPECT_TRUE(SDCreateDirectory(dirWithContent.c_str()));
    
    // Add subdirectory and files
    std::string subDir = dirWithContent + "/subdir";
    EXPECT_TRUE(SDCreateDirectory(subDir.c_str()));
    createTestFile(dirWithContent + "/file.txt", "content");
    createTestFile(subDir + "/nested.txt", "nested content");
    
    // Should fail to delete without force (directory not empty)
    EXPECT_FALSE(SDDeleteDirectory(dirWithContent.c_str(), FALSE));
    EXPECT_TRUE(SDDirectoryExists(dirWithContent.c_str()));
    
    // Should succeed with force
    EXPECT_TRUE(SDDeleteDirectory(dirWithContent.c_str(), TRUE));
    EXPECT_FALSE(SDDirectoryExists(dirWithContent.c_str()));
}

TEST_F(SDDirTest, EdgeCases) {
    // Test with empty string
    EXPECT_FALSE(SDCreateDirectory(""));
    EXPECT_FALSE(SDDirectoryExists(""));
    EXPECT_FALSE(SDIsDirectory(""));
    EXPECT_FALSE(SDIsEmptyDirectory(""));
    
    // Test with just a dot
    EXPECT_TRUE(SDDirectoryExists("."));
    EXPECT_TRUE(SDIsDirectory("."));
    
    // Test with current directory
    EXPECT_TRUE(SDDirectoryExists("./"));
    EXPECT_TRUE(SDIsDirectory("./"));
}

TEST_F(SDDirTest, UnicodeAndSpecialCharacters) {
    // Test with directory names containing special characters
    std::string specialDir = testRoot + "/dir with spaces";
    EXPECT_TRUE(SDCreateDirectory(specialDir.c_str()));
    EXPECT_TRUE(SDDirectoryExists(specialDir.c_str()));
    EXPECT_TRUE(SDIsDirectory(specialDir.c_str()));
    
    // Test reading directory with special characters
    CSDDirectory dir;
    EXPECT_TRUE(dir.Open(testRoot.c_str()));
    
    char name[256];
    SFileAttr attr;
    bool foundSpecialDir = false;
    
    while (dir.Read(name, sizeof(name), &attr)) {
        if (std::string(name) == "dir with spaces") {
            foundSpecialDir = true;
            EXPECT_TRUE(attr.isDir);
        }
    }
    
    dir.Close();
    EXPECT_TRUE(foundSpecialDir);
}

TEST_F(SDDirTest, LongPathNames) {
    // Test with reasonably long path names
    std::string longName = testRoot + "/this_is_a_very_long_directory_name_that_should_still_work_fine";
    EXPECT_TRUE(SDCreateDirectory(longName.c_str()));
    EXPECT_TRUE(SDDirectoryExists(longName.c_str()));
    
    // Test reading long names
    CSDDirectory dir;
    EXPECT_TRUE(dir.Open(testRoot.c_str()));
    
    char name[512];
    SFileAttr attr;
    bool foundLongName = false;
    
    while (dir.Read(name, sizeof(name), &attr)) {
        if (std::string(name).find("this_is_a_very_long_directory") != std::string::npos) {
            foundLongName = true;
            EXPECT_TRUE(attr.isDir);
        }
    }
    
    dir.Close();
    EXPECT_TRUE(foundLongName);
}

// Performance test
TEST_F(SDDirTest, PerformanceTest) {
    // Create many files and directories for performance testing
    const int numFiles = 100;
    const int numDirs = 50;
    
    std::string perfTestDir = testRoot + "/perf_test";
    EXPECT_TRUE(SDCreateDirectory(perfTestDir.c_str()));
    
    auto start = std::chrono::high_resolution_clock::now();
    
    // Create files and directories
    for (int i = 0; i < numFiles; ++i) {
        createTestFile(perfTestDir + "/file" + std::to_string(i) + ".txt", "test content");
    }
    
    for (int i = 0; i < numDirs; ++i) {
        std::string dirName = perfTestDir + "/dir" + std::to_string(i);
        EXPECT_TRUE(SDCreateDirectory(dirName.c_str()));
    }
    
    auto createTime = std::chrono::high_resolution_clock::now() - start;
    
    // Read all entries
    start = std::chrono::high_resolution_clock::now();
    
    CSDDirectory dir;
    EXPECT_TRUE(dir.Open(perfTestDir.c_str()));
    
    char name[256];
    SFileAttr attr;
    int entryCount = 0;
    
    while (dir.Read(name, sizeof(name), &attr)) {
        entryCount++;
    }
    
    dir.Close();
    
    auto readTime = std::chrono::high_resolution_clock::now() - start;
    
    EXPECT_EQ(entryCount, numFiles + numDirs);
    
    auto createMicros = std::chrono::duration_cast<std::chrono::microseconds>(createTime).count();
    auto readMicros = std::chrono::duration_cast<std::chrono::microseconds>(readTime).count();
    
    std::cout << "Directory performance test:\n";
    std::cout << "Created " << (numFiles + numDirs) << " entries in " << createMicros << " μs\n";
    std::cout << "Read " << entryCount << " entries in " << readMicros << " μs\n";
    
    // Performance should be reasonable
    EXPECT_LT(createMicros, 10000000); // Less than 10 seconds
    EXPECT_LT(readMicros, 1000000);    // Less than 1 second
}