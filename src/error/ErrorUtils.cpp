#include "error/ErrorUtils.hpp"
#include "logging.hpp"
#include <iostream>

namespace dbbackup {
namespace error {

void logError(const std::string& component, const std::string& message) {
    // Get logger and log the error
    auto logger = getLogger();
    logger->error("[{}] {}", component, message);
    
    // Also print to stderr for immediate visibility
    std::cerr << "[" << component << "] Error: " << message << std::endl;
}

} // namespace error
} // namespace dbbackup 