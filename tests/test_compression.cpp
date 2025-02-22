#include <gtest/gtest.h>
#include "../include/compression.hpp"
#include "../include/config.hpp"
#include "../include/error/DatabaseBackupError.hpp"
#include <filesystem>
#include <fstream>
#include <random>
#include <vector>

using namespace dbbackup;
using namespace dbbackup::error;
namespace fs = std::filesystem;

// Helper function to create a test file with compressible content
void createTestFile(const std::string& path, size_t size) {
    std::ofstream file(path, std::ios::binary);
    std::vector<char> data(size);
    
    // Create a repeating pattern that is highly compressible
    const std::string pattern = "This is a test pattern that should be highly compressible when repeated multiple times. ";
    size_t patternPos = 0;
    
    for (size_t i = 0; i < size; i++) {
        data[i] = pattern[patternPos];
        patternPos = (patternPos + 1) % pattern.length();
    }
    
    file.write(data.data(), data.size());
}

// Helper function to read file content
std::vector<char> readFileContent(const std::string& path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<char> buffer(size);
    file.read(buffer.data(), size);
    return buffer;
}

// Helper function to create test file with random (incompressible) content
void createRandomFile(const std::string& path, size_t size) {
    std::ofstream file(path, std::ios::binary);
    std::vector<char> data(size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    for (auto& byte : data) {
        byte = static_cast<char>(dis(gen));
    }
    file.write(data.data(), data.size());
}

class CompressionTest : public ::testing::Test {
protected:
    void SetUp() override {
        testDir = fs::temp_directory_path() / "compression_test";
        fs::create_directory(testDir);
    }

    void TearDown() override {
        fs::remove_all(testDir);
    }

    fs::path testDir;
};

TEST_F(CompressionTest, CompressAndDecompressSmallFile) {
    fs::path inputPath = testDir / "input.txt";
    fs::path compressedPath = testDir / "compressed.gz";
    fs::path decompressedPath = testDir / "decompressed.txt";

    // Create a small test file (1KB)
    createTestFile(inputPath.string(), 1024);

    // Configure compression
    CompressionConfig config;
    config.enabled = true;
    config.format = "gzip";
    config.level = "medium";

    Compressor compressor(config);

    // Compress the file
    EXPECT_TRUE(compressor.compressFile(inputPath.string(), compressedPath.string()));
    EXPECT_TRUE(fs::exists(compressedPath));

    // Decompress the file
    EXPECT_TRUE(compressor.decompressFile(compressedPath.string(), decompressedPath.string()));
    EXPECT_TRUE(fs::exists(decompressedPath));

    // Verify content matches
    auto originalContent = readFileContent(inputPath.string());
    auto decompressedContent = readFileContent(decompressedPath.string());
    EXPECT_EQ(originalContent, decompressedContent);
}

TEST_F(CompressionTest, CompressAndDecompressLargeFile) {
    fs::path inputPath = testDir / "large_input.txt";
    fs::path compressedPath = testDir / "large_compressed.gz";
    fs::path decompressedPath = testDir / "large_decompressed.txt";

    // Create a larger test file (5MB)
    createTestFile(inputPath.string(), 5 * 1024 * 1024);

    // Configure compression
    CompressionConfig config;
    config.enabled = true;
    config.format = "gzip";
    config.level = "high";

    Compressor compressor(config);

    // Compress the file
    EXPECT_TRUE(compressor.compressFile(inputPath.string(), compressedPath.string()));
    EXPECT_TRUE(fs::exists(compressedPath));

    // Verify compressed file is smaller
    EXPECT_LT(fs::file_size(compressedPath), fs::file_size(inputPath));

    // Decompress the file
    EXPECT_TRUE(compressor.decompressFile(compressedPath.string(), decompressedPath.string()));
    EXPECT_TRUE(fs::exists(decompressedPath));

    // Verify content matches
    auto originalContent = readFileContent(inputPath.string());
    auto decompressedContent = readFileContent(decompressedPath.string());
    EXPECT_EQ(originalContent, decompressedContent);
}

TEST_F(CompressionTest, CompressionLevels) {
    fs::path inputPath = testDir / "input.txt";
    fs::path compressedLowPath = testDir / "compressed_low.gz";
    fs::path compressedHighPath = testDir / "compressed_high.gz";

    // Create a test file with compressible content (1MB)
    createTestFile(inputPath.string(), 1024 * 1024);

    // Test with low compression
    CompressionConfig lowConfig;
    lowConfig.enabled = true;
    lowConfig.format = "gzip";
    lowConfig.level = "low";
    Compressor lowCompressor(lowConfig);
    EXPECT_TRUE(lowCompressor.compressFile(inputPath.string(), compressedLowPath.string()));

    // Test with high compression
    CompressionConfig highConfig;
    highConfig.enabled = true;
    highConfig.format = "gzip";
    highConfig.level = "high";
    Compressor highCompressor(highConfig);
    EXPECT_TRUE(highCompressor.compressFile(inputPath.string(), compressedHighPath.string()));

    // High compression should result in smaller file size
    EXPECT_LT(fs::file_size(compressedHighPath), fs::file_size(compressedLowPath));
}

TEST_F(CompressionTest, InvalidInputFile) {
    fs::path nonExistentFile = testDir / "nonexistent.txt";
    fs::path outputPath = testDir / "output.gz";

    CompressionConfig config;
    config.enabled = true;
    config.format = "gzip";
    config.level = "medium";

    Compressor compressor(config);
    EXPECT_THROW(compressor.compressFile(nonExistentFile.string(), outputPath.string()), CompressionError);
}

TEST_F(CompressionTest, InvalidCompressionFormat) {
    CompressionConfig config;
    config.enabled = true;
    config.format = "invalid";
    config.level = "medium";

    EXPECT_THROW(Compressor compressor(config), ConfigurationError);
}

TEST_F(CompressionTest, InvalidCompressionLevel) {
    CompressionConfig config;
    config.enabled = true;
    config.format = "gzip";
    config.level = "invalid";

    EXPECT_THROW(Compressor compressor(config), ConfigurationError);
}

TEST_F(CompressionTest, EstimateCompressedSize) {
    CompressionConfig config;
    config.enabled = true;
    config.format = "gzip";
    config.level = "medium";

    Compressor compressor(config);
    
    // Test with incompressible data (random)
    size_t inputSize = 1024 * 1024; // 1MB
    fs::path randomPath = testDir / "random_input.txt";
    fs::path randomCompressedPath = testDir / "random_compressed.gz";
    createRandomFile(randomPath.string(), inputSize);
    
    size_t estimatedSize = compressor.estimateCompressedSize(inputSize);
    EXPECT_TRUE(compressor.compressFile(randomPath.string(), randomCompressedPath.string()));
    
    // For random data, compressed size should be close to estimated size
    size_t actualRandomSize = fs::file_size(randomCompressedPath);
    double randomRatio = static_cast<double>(actualRandomSize) / estimatedSize;
    EXPECT_GT(randomRatio, 0.8);  // Allow 20% variance below estimate
    EXPECT_LT(randomRatio, 1.2);  // Allow 20% variance above estimate
    
    // Test with compressible data (pattern)
    fs::path patternPath = testDir / "pattern_input.txt";
    fs::path patternCompressedPath = testDir / "pattern_compressed.gz";
    createTestFile(patternPath.string(), inputSize);
    
    EXPECT_TRUE(compressor.compressFile(patternPath.string(), patternCompressedPath.string()));
    
    // For compressible data, actual size can be much smaller than estimated
    size_t actualPatternSize = fs::file_size(patternCompressedPath);
    EXPECT_LT(actualPatternSize, estimatedSize);  // Should compress better than estimated
} 