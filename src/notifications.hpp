#pragma once

#include "config.hpp"

/// Sends a notification if Slack (or other) is configured and enabled.
void sendNotificationIfNeeded(const LoggingConfig& loggingConfig, const std::string& message);
