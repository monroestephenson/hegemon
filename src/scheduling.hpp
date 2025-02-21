#pragma once

#include "config.hpp"
#include <atomic>
#include <thread>

/// A simple internal scheduler example. 
/// In production, you'd likely use Cron, Task Scheduler, etc.
class Scheduler {
public:
    Scheduler(const Config& cfg);
    ~Scheduler();

    void start();
    void stop();

private:
    void runScheduler();

    Config m_config;
    std::atomic_bool m_running;
    std::thread m_thread;
};
