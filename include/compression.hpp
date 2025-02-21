#pragma once

#include "config.hpp"
#include "error/DatabaseBackupError.hpp"
#include <string>
#include <cstddef>

namespace dbbackup {

enum class CompressionLevel {
    Low,
    Medium,
    High
};

enum class CompressionFormat {
    Gzip,
    Bzip2,
    Xz
};

class Compressor {
public:
    explicit Compressor(const CompressionConfig& config);

    /// Compresses the file at inputPath, writes to outputPath.
    /// Returns true on success.
    bool compressFile(const std::string& inputPath, const std::string& outputPath) const;

    /// Decompresses the file at inputPath, writes to outputPath.
    /// Returns true on success.
    bool decompressFile(const std::string& inputPath, const std::string& outputPath) const;

    /// Get the estimated compressed size for a given input size
    size_t estimateCompressedSize(size_t inputSize) const;

    /// Get the file extension for the current compression format
    std::string getFileExtension() const;

private:
    CompressionFormat format;
    CompressionLevel level;
    
    // Helper functions for different compression formats
    bool compressGzip(const std::string& inputPath, const std::string& outputPath) const;
    bool decompressGzip(const std::string& inputPath, const std::string& outputPath) const;
    
    // Convert string format to enum
    static CompressionFormat stringToFormat(const std::string& format);
    static CompressionLevel stringToLevel(const std::string& level);
    
    // Get compression level for zlib based on CompressionLevel
    int getZlibLevel() const;
};

} // namespace dbbackup 