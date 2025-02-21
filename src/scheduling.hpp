#pragma once

#include "config.hpp"
#include <atomic>
#include <thread>

/// A simple internal scheduler example. 
/// In production, you'd likely use Cron, Task Scheduler, etc.
class Scheduler {
public:
    Scheduler(const dbbackup::Config& cfg);
    ~Scheduler();

    void start();
    void stop();

private:
    void runScheduler();

    dbbackup::Config m_config;
    std::atomic_bool m_running;
    std::thread m_thread;
};
