#include "../include/compression.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <zlib.h>
#include <vector>
#include <stdexcept>

namespace fs = std::filesystem;
using namespace dbbackup::error;

namespace dbbackup {

namespace {
    constexpr size_t CHUNK_SIZE = 16384;  // 16KB chunks for reading/writing
}

Compressor::Compressor(const CompressionConfig& config)
    : format(stringToFormat(config.format))
    , level(stringToLevel(config.level)) {
}

CompressionFormat Compressor::stringToFormat(const std::string& format) {
    if (format == "gzip") return CompressionFormat::Gzip;
    if (format == "bzip2") return CompressionFormat::Bzip2;
    if (format == "xz") return CompressionFormat::Xz;
    DB_THROW(ConfigurationError, "Unsupported compression format: " + format);
}

CompressionLevel Compressor::stringToLevel(const std::string& level) {
    if (level == "low") return CompressionLevel::Low;
    if (level == "medium") return CompressionLevel::Medium;
    if (level == "high") return CompressionLevel::High;
    DB_THROW(ConfigurationError, "Unsupported compression level: " + level);
}

int Compressor::getZlibLevel() const {
    switch (level) {
        case CompressionLevel::Low: return 1;
        case CompressionLevel::Medium: return 6;
        case CompressionLevel::High: return 9;
        default: return 6;
    }
}

std::string Compressor::getFileExtension() const {
    switch (format) {
        case CompressionFormat::Gzip: return ".gz";
        case CompressionFormat::Bzip2: return ".bz2";
        case CompressionFormat::Xz: return ".xz";
        default: return ".gz";
    }
}

bool Compressor::compressFile(const std::string& inputPath, const std::string& outputPath) const {
    switch (format) {
        case CompressionFormat::Gzip:
            return compressGzip(inputPath, outputPath);
        case CompressionFormat::Bzip2:
        case CompressionFormat::Xz:
            DB_THROW(ConfigurationError, "Compression format not yet implemented");
        default:
            DB_THROW(ConfigurationError, "Unknown compression format");
    }
    return false;
}

bool Compressor::decompressFile(const std::string& inputPath, const std::string& outputPath) const {
    switch (format) {
        case CompressionFormat::Gzip:
            return decompressGzip(inputPath, outputPath);
        case CompressionFormat::Bzip2:
        case CompressionFormat::Xz:
            DB_THROW(ConfigurationError, "Decompression format not yet implemented");
        default:
            DB_THROW(ConfigurationError, "Unknown compression format");
    }
    return false;
}

bool Compressor::compressGzip(const std::string& inputPath, const std::string& outputPath) const {
    DB_TRY_CATCH_LOG("Compression", {
        std::ifstream inFile(inputPath, std::ios::binary);
        if (!inFile) {
            DB_THROW(CompressionError, "Failed to open input file for compression");
        }

        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile) {
            DB_THROW(CompressionError, "Failed to open output file for compression");
        }

        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;

        int ret = deflateInit2(&stream, getZlibLevel(), Z_DEFLATED,
                             15 + 16,  // 15 window bits + 16 for gzip header
                             8,        // memory level
                             Z_DEFAULT_STRATEGY);
        
        if (ret != Z_OK) {
            DB_THROW(CompressionError, "Failed to initialize compression");
        }

        std::vector<unsigned char> inBuffer(CHUNK_SIZE);
        std::vector<unsigned char> outBuffer(CHUNK_SIZE);

        do {
            inFile.read(reinterpret_cast<char*>(inBuffer.data()), CHUNK_SIZE);
            stream.avail_in = inFile.gcount();
            stream.next_in = inBuffer.data();

            do {
                stream.avail_out = CHUNK_SIZE;
                stream.next_out = outBuffer.data();

                ret = deflate(&stream, inFile.eof() ? Z_FINISH : Z_NO_FLUSH);
                if (ret == Z_STREAM_ERROR) {
                    deflateEnd(&stream);
                    DB_THROW(CompressionError, "Compression error");
                }

                int have = CHUNK_SIZE - stream.avail_out;
                outFile.write(reinterpret_cast<char*>(outBuffer.data()), have);
                
                if (!outFile) {
                    deflateEnd(&stream);
                    DB_THROW(CompressionError, "Failed to write compressed data");
                }
            } while (stream.avail_out == 0);
        } while (!inFile.eof());

        deflateEnd(&stream);
        return true;
    });
    return false;
}

bool Compressor::decompressGzip(const std::string& inputPath, const std::string& outputPath) const {
    DB_TRY_CATCH_LOG("Compression", {
        std::ifstream inFile(inputPath, std::ios::binary);
        if (!inFile) {
            DB_THROW(CompressionError, "Failed to open input file for decompression");
        }

        std::ofstream outFile(outputPath, std::ios::binary);
        if (!outFile) {
            DB_THROW(CompressionError, "Failed to open output file for decompression");
        }

        z_stream stream;
        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;
        stream.avail_in = 0;
        stream.next_in = Z_NULL;

        int ret = inflateInit2(&stream, 15 + 16);  // 15 window bits + 16 for gzip header
        if (ret != Z_OK) {
            DB_THROW(CompressionError, "Failed to initialize decompression");
        }

        std::vector<unsigned char> inBuffer(CHUNK_SIZE);
        std::vector<unsigned char> outBuffer(CHUNK_SIZE);

        do {
            inFile.read(reinterpret_cast<char*>(inBuffer.data()), CHUNK_SIZE);
            stream.avail_in = inFile.gcount();
            
            if (stream.avail_in == 0) {
                break;
            }
            stream.next_in = inBuffer.data();

            do {
                stream.avail_out = CHUNK_SIZE;
                stream.next_out = outBuffer.data();

                ret = inflate(&stream, Z_NO_FLUSH);
                switch (ret) {
                    case Z_NEED_DICT:
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                        inflateEnd(&stream);
                        DB_THROW(CompressionError, "Decompression error");
                }

                int have = CHUNK_SIZE - stream.avail_out;
                outFile.write(reinterpret_cast<char*>(outBuffer.data()), have);
                
                if (!outFile) {
                    inflateEnd(&stream);
                    DB_THROW(CompressionError, "Failed to write decompressed data");
                }
            } while (stream.avail_out == 0);
        } while (ret != Z_STREAM_END);

        inflateEnd(&stream);
        
        if (ret != Z_STREAM_END) {
            DB_THROW(CompressionError, "Incomplete or corrupted compressed data");
        }
        
        return true;
    });
    return false;
}

size_t Compressor::estimateCompressedSize(size_t inputSize) const {
    // Conservative estimation based on compression level and format
    // For random/incompressible data, compression might actually increase size slightly
    double ratio;
    switch (level) {
        case CompressionLevel::Low:
            ratio = 1.0;  // Assume no compression for low level
            break;
        case CompressionLevel::Medium:
            ratio = 0.9;  // Expect ~10% compression at best
            break;
        case CompressionLevel::High:
            ratio = 0.8;  // Expect ~20% compression at best
            break;
        default:
            ratio = 0.9;
    }
    
    // Add overhead for gzip headers and such
    size_t overhead = 1024;  // 1KB overhead
    return static_cast<size_t>(inputSize * ratio) + overhead;
}

bool compressFile(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile) {
        return false;
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        return false;
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;

    // Use maximum compression level and best strategy
    if (deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 31, 9, Z_FILTERED) != Z_OK) {
        return false;
    }

    // Use a larger buffer for better compression
    std::vector<char> inBuffer(262144);  // 256KB
    std::vector<char> outBuffer(262144); // 256KB

    do {
        inFile.read(inBuffer.data(), inBuffer.size());
        zs.avail_in = static_cast<uInt>(inFile.gcount());
        zs.next_in = reinterpret_cast<Bytef*>(inBuffer.data());

        do {
            zs.avail_out = static_cast<uInt>(outBuffer.size());
            zs.next_out = reinterpret_cast<Bytef*>(outBuffer.data());

            int ret = deflate(&zs, inFile.eof() ? Z_FINISH : Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR) {
                deflateEnd(&zs);
                return false;
            }

            outFile.write(outBuffer.data(), outBuffer.size() - zs.avail_out);
        } while (zs.avail_out == 0);
    } while (!inFile.eof());

    deflateEnd(&zs);
    return true;
}

bool decompressFile(const std::string& inputPath, const std::string& outputPath) {
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile) {
        return false;
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        return false;
    }

    z_stream zs;
    zs.zalloc = Z_NULL;
    zs.zfree = Z_NULL;
    zs.opaque = Z_NULL;
    zs.avail_in = 0;
    zs.next_in = Z_NULL;

    if (inflateInit2(&zs, 31) != Z_OK) {
        return false;
    }

    std::vector<char> inBuffer(32768);
    std::vector<char> outBuffer(32768);

    do {
        inFile.read(inBuffer.data(), inBuffer.size());
        zs.avail_in = static_cast<uInt>(inFile.gcount());
        zs.next_in = reinterpret_cast<Bytef*>(inBuffer.data());

        do {
            zs.avail_out = static_cast<uInt>(outBuffer.size());
            zs.next_out = reinterpret_cast<Bytef*>(outBuffer.data());

            int ret = inflate(&zs, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR || ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
                inflateEnd(&zs);
                return false;
            }

            outFile.write(outBuffer.data(), outBuffer.size() - zs.avail_out);
        } while (zs.avail_out == 0);
    } while (inFile.good() && !inFile.eof());

    inflateEnd(&zs);
    return true;
}

} // namespace dbbackup
