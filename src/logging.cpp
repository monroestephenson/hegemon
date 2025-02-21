#include "logging.hpp"
#include <spdlog/sinks/basic_file_sink.h>
#include <mutex>

std::shared_ptr<spdlog::logger> getLogger() {
    static std::shared_ptr<spdlog::logger> logger;
    static std::once_flag onceFlag;

    std::call_once(onceFlag, [](){
        // In real scenario, you might read from config for log file name
        logger = spdlog::basic_logger_mt("backup_logger", "backup.log");
        logger->set_level(spdlog::level::info);
    });

    return logger;
}
