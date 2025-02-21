#pragma once

#include "config.hpp"
#include <string>

/// Sends a notification if Slack (or other) is configured and enabled.
void sendNotificationIfNeeded(const dbbackup::LoggingConfig& loggingConfig, const std::string& message);
