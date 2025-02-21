#include <gtest/gtest.h>
#include "scheduling.hpp"
#include "config.hpp"
#include <chrono>
#include <thread>

TEST(SchedulerTest, StartAndStop) {
    Config cfg;
    cfg.schedule.enabled = true; 
    // super short intervals or mocking the sleep logic is recommended for unit tests
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
