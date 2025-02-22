#pragma once

#include "config.hpp"
#include <string>

class RestoreManager {
public:
    explicit RestoreManager(const dbbackup::Config& config);
    bool restore(const std::string& backupPath);

private:
    dbbackup::Config m_config;
}; 