#include <cli.hpp>
#include "config.hpp"
#include "backup_manager.hpp"
#include "restore_manager.hpp"
#include "error/ErrorUtils.hpp"
#include <iostream>
#include <memory>
#include <filesystem>
#include <iomanip>
#include <ctime>

using namespace dbbackup::error;

// Helper function to format file size
std::string formatSize(uintmax_t size) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double dsize = static_cast<double>(size);
    
    while (dsize >= 1024.0 && unit < 3) {
        dsize /= 1024.0;
        unit++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << dsize << " " << units[unit];
    return ss.str();
}

// Helper function to format timestamp from filename
std::string formatTimestamp(const std::string& filename) {
    // Extract timestamp from format: backup_YYYYMMDD_HHMMSS_type.dump.gz
    size_t start = filename.find("_") + 1;
    if (start == std::string::npos) return "Unknown";
    
    std::string timestamp = filename.substr(start, 15); // YYYYMMDD_HHMMSS
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y%m%d_%H%M%S");
    
    if (ss.fail()) return "Unknown";
    
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
    return buffer;
}

// Helper function to list backups
void listBackups(const std::string& backupDir) {
    if (!std::filesystem::exists(backupDir)) {
        std::cout << "No backups found (directory does not exist)\n";
        return;
    }

    std::vector<std::filesystem::directory_entry> backups;
    for (const auto& entry : std::filesystem::directory_iterator(backupDir)) {
        if (entry.is_regular_file() && entry.path().filename().string().starts_with("backup_")) {
            backups.push_back(entry);
        }
    }

    if (backups.empty()) {
        std::cout << "No backups found\n";
        return;
    }

    // Sort backups by modification time (newest first)
    std::sort(backups.begin(), backups.end(),
              [](const auto& a, const auto& b) {
                  return a.last_write_time() > b.last_write_time();
              });

    // Print header
    std::cout << std::left
              << std::setw(25) << "Timestamp"
              << std::setw(10) << "Type"
              << std::setw(15) << "Size"
              << "Filename\n"
              << std::string(70, '-') << "\n";

    // Print each backup
    for (const auto& backup : backups) {
        std::string filename = backup.path().filename().string();
        
        // Extract backup type
        std::string type = "full";
        size_t typeStart = filename.find_last_of("_") + 1;
        size_t typeEnd = filename.find(".dump");
        if (typeStart != std::string::npos && typeEnd != std::string::npos) {
            type = filename.substr(typeStart, typeEnd - typeStart);
        }

        std::cout << std::left
                  << std::setw(25) << formatTimestamp(filename)
                  << std::setw(10) << type
                  << std::setw(15) << formatSize(backup.file_size())
                  << filename << "\n";
    }
}

// Helper function to verify backup
bool verifyBackup(const std::string& backupPath, const dbbackup::Config& config) {
    std::cout << "Verifying backup: " << backupPath << "\n";
    
    // Check if file exists
    if (!std::filesystem::exists(backupPath)) {
        std::cerr << "Error: Backup file not found\n";
        return false;
    }

    // Check file size
    uintmax_t size = std::filesystem::file_size(backupPath);
    if (size == 0) {
        std::cerr << "Error: Backup file is empty\n";
        return false;
    }
    std::cout << "File size: " << formatSize(size) << "\n";

    // Check if it's a gzip file
    bool isGzip = false;
    {
        std::ifstream file(backupPath, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Cannot open backup file\n";
            return false;
        }

        unsigned char header[2];
        file.read(reinterpret_cast<char*>(header), 2);
        isGzip = (file && header[0] == 0x1f && header[1] == 0x8b);
    }

    std::cout << "Compression: " << (isGzip ? "gzip" : "none") << "\n";

    // For gzipped files, try to decompress to verify integrity
    if (isGzip) {
        std::cout << "Verifying gzip integrity...\n";
        std::unique_ptr<dbbackup::Compressor> compressor = 
            std::make_unique<dbbackup::Compressor>(config.backup.compression);
        
        std::string tempPath = backupPath + ".verify";
        bool decompressSuccess = compressor->decompressFile(backupPath, tempPath);
        
        if (decompressSuccess) {
            std::cout << "Decompression successful\n";
            std::filesystem::remove(tempPath);
        } else {
            std::cerr << "Error: Failed to decompress file\n";
            return false;
        }
    }

    // Extract and verify timestamp from filename
    std::string filename = std::filesystem::path(backupPath).filename().string();
    std::string timestamp = formatTimestamp(filename);
    if (timestamp == "Unknown") {
        std::cerr << "Warning: Invalid timestamp format in filename\n";
    } else {
        std::cout << "Timestamp: " << timestamp << "\n";
    }

    // Extract and verify backup type
    std::string type = "unknown";
    size_t typeStart = filename.find_last_of("_") + 1;
    size_t typeEnd = filename.find(".dump");
    if (typeStart != std::string::npos && typeEnd != std::string::npos) {
        type = filename.substr(typeStart, typeEnd - typeStart);
    }
    
    if (type != "full" && type != "incremental" && type != "differential") {
        std::cerr << "Warning: Unknown backup type: " << type << "\n";
    } else {
        std::cout << "Backup type: " << type << "\n";
    }

    std::cout << "Verification completed successfully\n";
    return true;
}

int main(int argc, char* argv[]) {
    try {
        // Parse command line arguments
        CLI cli(argc, argv);
        CLIOptions options = cli.parse();

        // Load configuration from file
        dbbackup::Config config = dbbackup::Config::fromFile(options.configPath);

        // Override config with command line options if provided
        if (!options.dbType.empty()) {
            config.database.type = options.dbType == "postgres" ? "postgresql" : options.dbType;
        }
        
        if (!options.dbHost.empty()) {
            config.database.host = options.dbHost;
        }
        
        if (options.dbPort != 0) {
            config.database.port = options.dbPort;
        }
        
        if (!options.dbUser.empty()) {
            config.database.username = options.dbUser;
        }
        
        if (!options.dbName.empty()) {
            config.database.database = options.dbName;
        }
        
        if (!options.dbFile.empty() && config.database.type == "sqlite") {
            config.database.database = options.dbFile;
        }

        // Override backup settings if provided
        if (!options.compression.empty()) {
            config.backup.compression.enabled = (options.compression != "none");
            if (config.backup.compression.enabled) {
                config.backup.compression.format = options.compression;
            }
        }

        // Override logging settings
        if (options.verbose) {
            config.logging.logLevel = "debug";
        }

        // Execute the appropriate command
        if (options.command == "backup") {
            BackupManager backupMgr(config);
            if (!backupMgr.backup(options.backupType)) {
                return 1;
            }
        }
        else if (options.command == "restore") {
            RestoreManager restoreMgr(config);
            if (!restoreMgr.restore(options.restorePath)) {
                return 1;
            }
        }
        else if (options.command == "list") {
            listBackups(config.storage.localPath);
        }
        else if (options.command == "verify") {
            if (!verifyBackup(options.restorePath, config)) {
                return 1;
            }
        }
        else {
            std::cerr << "Error: No command specified. Use -h or --help for usage information.\n";
            return 1;
        }

        return 0;
    }
    catch (const DatabaseBackupError& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << std::endl;
        return 1;
    }
}
