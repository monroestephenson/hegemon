#include "notifications.hpp"
#include <iostream>
// #include <curl/curl.h> // Use libcurl for Slack webhooks

void sendNotificationIfNeeded(const LoggingConfig& loggingConfig, const std::string& message) {
    if(!loggingConfig.enableSlack || loggingConfig.slackWebhookUrl.empty()) {
        return;
    }

    // Stub: In a real scenario, use libcurl to POST to Slack Webhook.
    std::cout << "[Notification] Sending Slack message to " 
              << loggingConfig.slackWebhookUrl << ": " << message << "\n";
}
