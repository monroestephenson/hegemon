#pragma once

#include <memory>
#include <spdlog/spdlog.h>

/// Initializes and/or returns a global or singleton logger instance
std::shared_ptr<spdlog::logger> getLogger();
