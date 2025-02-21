#pragma once

#include <sstream>
#include <string>
#include "error/DatabaseBackupError.hpp"

namespace dbbackup {
namespace error {

// Helper function to create detailed error messages
inline std::string makeErrorMessage(const char* file, int line, const std::string& message) {
    std::ostringstream oss;
    oss << "[" << file << ":" << line << "] " << message;
    return oss.str();
}

// Helper function to handle and log errors
void logError(const std::string& component, const std::string& message);

} // namespace error
} // namespace dbbackup

// Error handling macros
#define DB_THROW(exception_type, message) \
    throw exception_type(dbbackup::error::makeErrorMessage(__FILE__, __LINE__, message))

#define DB_CHECK(condition, exception_type, message) \
    do { \
        if (!(condition)) { \
            DB_THROW(exception_type, message); \
        } \
    } while (0)

#define DB_TRY_CATCH_LOG(component, code) \
    try { \
        code \
    } catch (const std::exception& e) { \
        dbbackup::error::logError(component, e.what()); \
        throw; \
    } 