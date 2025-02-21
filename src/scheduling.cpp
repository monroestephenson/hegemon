#include "scheduling.hpp"
#include "backup_manager.hpp"
#include "logging.hpp"
#include <chrono>
#include <thread>
#include <ctime>

using namespace dbbackup;

/// A simple internal scheduler example. 
/// In production, you'd likely use Cron, Task Scheduler, etc.
Scheduler::Scheduler(const dbbackup::Config& cfg)
    : m_config(cfg)
    , m_running(false)
{
}

Scheduler::~Scheduler() {
    stop();
}

void Scheduler::start() {
    if (!m_running) {
        m_running = true;
        m_thread = std::thread(&Scheduler::runScheduler, this);
    }
}

void Scheduler::stop() {
    if (m_running) {
        m_running = false;
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

bool isCronMatch(const std::string& cron, const std::tm& tm) {
    // Simple cron implementation for daily backups at specific hour
    // Format: "0 HOUR * * *" where HOUR is 0-23
    if (cron.length() >= 5) {
        size_t spacePos = cron.find(' ');
        if (spacePos != std::string::npos) {
            std::string hourStr = cron.substr(spacePos + 1);
            size_t nextSpace = hourStr.find(' ');
            if (nextSpace != std::string::npos) {
                hourStr = hourStr.substr(0, nextSpace);
                try {
                    int hour = std::stoi(hourStr);
                    return tm.tm_hour == hour && tm.tm_min == 0;
                } catch (...) {
                    return false;
                }
            }
        }
    }
    return false;
}

void Scheduler::runScheduler() {
    auto logger = getLogger();
    logger->info("Scheduler started");

    BackupManager backupManager(m_config);
    time_t lastBackupTime = 0;

    while (m_running) {
        try {
            // Get current time
            time_t now = std::time(nullptr);
            std::tm* tm = std::localtime(&now);
            
            // Check if it's time for a backup based on schedule configuration
            if (m_config.backup.schedule.enabled) {
                std::string cron = m_config.backup.schedule.cron;
                
                // Check if current time matches the cron schedule
                if (isCronMatch(cron, *tm)) {
                    // Only run if we haven't already done a backup in this hour
                    time_t hourStart = now - (tm->tm_min * 60 + tm->tm_sec);
                    if (lastBackupTime < hourStart) {
                        logger->info("Starting scheduled backup");
                        
                        // Perform a full backup by default
                        if (backupManager.backup("full")) {
                            logger->info("Scheduled backup completed successfully");
                            lastBackupTime = now;
                        } else {
                            logger->error("Scheduled backup failed");
                        }
                    }
                }
            }
            
            // Sleep for a minute before checking again
            std::this_thread::sleep_for(std::chrono::minutes(1));
            
        } catch (const std::exception& e) {
            logger->error("Error in scheduler: {}", e.what());
            // Sleep for a minute before retrying
            std::this_thread::sleep_for(std::chrono::minutes(1));
        }
    }
    
    logger->info("Scheduler stopped");
}
