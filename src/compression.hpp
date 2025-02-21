#pragma once

#include <string>

/// Compresses the file at inputPath, writes to outputPath.
/// Returns true on success.
bool compressFile(const std::string& inputPath, const std::string& outputPath);

/// Decompresses the file at inputPath, writes to outputPath.
/// Returns true on success.
bool decompressFile(const std::string& inputPath, const std::string& outputPath);
