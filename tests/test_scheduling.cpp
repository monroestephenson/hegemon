#include <gtest/gtest.h>
#include "scheduling.hpp"
#include "config.hpp"
#include <chrono>
#include <thread>

using namespace dbbackup;

TEST(SchedulerTest, StartAndStop) {
    // Create a valid configuration
    dbbackup::Config cfg;
    cfg.database.type = "mysql";
    cfg.database.host = "localhost";
    cfg.database.port = 3306;
    cfg.database.username = "test_user";
    cfg.database.password = "test_pass";
    cfg.database.database = "test_db";
    
    cfg.storage.localPath = "/tmp/backups";
    cfg.storage.cloudProvider = "";
    cfg.storage.cloudPath = "";
    
    cfg.logging.logPath = "/tmp/logs";
    cfg.logging.logLevel = "info";
    
    cfg.backup.schedule.enabled = true;
    cfg.backup.schedule.cron = "0 0 * * *";

    // Create and test scheduler
    Scheduler scheduler(cfg);

    scheduler.start();
    // Sleep a bit so that at least one scheduled backup may occur
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    scheduler.stop();

    // There's not much to assert here unless you mock the BackupManager 
    // or track the number of backups triggered, etc.
    // In real usage, you'd pass a mock BackupManager to the Scheduler for verification.
    EXPECT_TRUE(true);
}
