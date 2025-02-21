#pragma once

#include <stdexcept>
#include <string>

namespace dbbackup {
namespace error {

// Base exception class for all database backup errors
class DatabaseBackupError : public std::runtime_error {
public:
    explicit DatabaseBackupError(const std::string& message) 
        : std::runtime_error(message) {}
};

// Connection related errors
class ConnectionError : public DatabaseBackupError {
public:
    explicit ConnectionError(const std::string& message) 
        : DatabaseBackupError("Connection error: " + message) {}
};

// Configuration related errors
class ConfigurationError : public DatabaseBackupError {
public:
    explicit ConfigurationError(const std::string& message) 
        : DatabaseBackupError("Configuration error: " + message) {}
};

// Backup operation errors
class BackupError : public DatabaseBackupError {
public:
    explicit BackupError(const std::string& message) 
        : DatabaseBackupError("Backup error: " + message) {}
};

// Restore operation errors
class RestoreError : public DatabaseBackupError {
public:
    explicit RestoreError(const std::string& message) 
        : DatabaseBackupError("Restore error: " + message) {}
};

// Storage related errors
class StorageError : public DatabaseBackupError {
public:
    explicit StorageError(const std::string& message) 
        : DatabaseBackupError("Storage error: " + message) {}
};

// Compression related errors
class CompressionError : public DatabaseBackupError {
public:
    explicit CompressionError(const std::string& message) 
        : DatabaseBackupError("Compression error: " + message) {}
};

// Authentication related errors
class AuthenticationError : public ConnectionError {
public:
    explicit AuthenticationError(const std::string& message) 
        : ConnectionError("Authentication failed: " + message) {}
};

// Validation errors
class ValidationError : public DatabaseBackupError {
public:
    explicit ValidationError(const std::string& message) 
        : DatabaseBackupError("Validation error: " + message) {}
};

} // namespace error
} // namespace dbbackup 