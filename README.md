# Database Backup CLI

A powerful and flexible command-line tool for backing up and restoring various types of databases. Written in modern C++, it supports multiple database systems, compression, and both local and cloud storage options.

## Quick Start

### Installation

#### macOS
```bash
# Install using Homebrew
brew tap monroestephenson/database-backup
brew install database-backup

# Optional: Install database-specific dependencies
brew install mysql-connector-c++ libpq mongo-cxx-driver sqlite3
```

#### Ubuntu/Debian
```bash
# Add our repository
curl -s https://packagecloud.io/install/repositories/monroestephenson/database-backup/script.deb.sh | sudo bash

# Install the package
sudo apt-get install database-backup

# Optional: Install database-specific dependencies
sudo apt-get install libmysqlcppconn-dev libpqxx-dev libmongocxx-dev libsqlite3-dev
```

#### Manual Installation
```bash
# Clone the repository
git clone https://github.com/monroestephenson/database_backup.git
cd database_backup

# Install
sudo make install
```

### Quick Usage

1. Set up your environment variables (add to ~/.zshrc or ~/.bash_profile):
```bash
export DB_USER=your_database_user
export DB_PASSWORD=your_database_password
export SLACK_WEBHOOK_ID=your_webhook_id  # Optional
export ENCRYPTION_KEY_PATH=/path/to/your/key  # Optional
```

2. Create your configuration:
```bash
# Copy the template
mkdir -p ~/.config/db-backup
cp /usr/local/etc/database_backup/config.template.json ~/.config/db-backup/config.json

# Edit the configuration
nano ~/.config/db-backup/config.json
```

3. Run backups:
```bash
# Perform a full backup
db-backup backup --type full

# Restore from backup
db-backup restore --file backup_20240221_123456.dump.gz

# Start scheduled backups
db-backup schedule
```

## Features

- 🗄️ **Multiple Database Support**
  - MySQL
  - PostgreSQL
  - MongoDB
  - SQLite
- 🔄 **Backup Types**
  - Full backups
  - Incremental backups (planned)
  - Differential backups (planned)
- 🔐 **Security**
  - Environment variable support for sensitive data
  - AES-256-GCM encryption support
  - Secure credential management
- 🗜️ **Compression**
  - Multiple formats (gzip, bzip2, xz)
  - Configurable compression levels
- 📦 **Storage Options**
  - Local storage
  - Cloud storage support
    - AWS S3
    - Google Cloud Storage (planned)
    - Azure Blob Storage (planned)
- ⏰ **Scheduling**
  - Cron-based scheduling
  - Retention policies
  - Backup rotation
- 📢 **Notifications**
  - Slack integration
  - Email notifications (planned)
- 📝 **Logging**
  - Comprehensive logging
  - Multiple log levels
  - Log rotation

## Prerequisites

- C++17 compatible compiler
- CMake 3.10 or higher
- Required libraries:
  - spdlog
  - CLI11
  - nlohmann_json
  - Database-specific client libraries (MySQL, PostgreSQL, etc.)

### Installing Dependencies

#### macOS
```bash
brew install cmake spdlog cli11 nlohmann-json
# Database-specific installations
brew install mysql-connector-c++  # For MySQL
brew install libpq               # For PostgreSQL
brew install mongo-cxx-driver    # For MongoDB
brew install sqlite3            # For SQLite
```

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake libspdlog-dev nlohmann-json3-dev
# Database-specific installations
sudo apt-get install libmysqlcppconn-dev  # For MySQL
sudo apt-get install libpqxx-dev          # For PostgreSQL
sudo apt-get install libmongocxx-dev      # For MongoDB
sudo apt-get install libsqlite3-dev       # For SQLite
```

## Configuration

### Environment Variables

The following environment variables need to be set for secure operation:

```bash
# Database credentials
export DB_USER=your_database_user
export DB_PASSWORD=your_database_password

# For Slack notifications (optional)
export SLACK_WEBHOOK_ID=your_slack_webhook_id

# For encryption (if enabled)
export ENCRYPTION_KEY_PATH=/path/to/your/encryption.key
```

### Configuration File

Create a `config.json` file in your project directory. Here's a sample configuration:

```json
{
    "database": {
        "type": "mysql",
        "host": "localhost",
        "port": 3306,
        "username": "${DB_USER}",
        "password": "${DB_PASSWORD}",
        "database": "mydb"
    },
    "storage": {
        "localPath": "./backups",
        "cloudProvider": "aws",
        "cloudPath": "my-backup-bucket/database-backups"
    },
    "logging": {
        "logPath": "./logs/backup.log",
        "logLevel": "info",
        "enableNotifications": true,
        "notificationEndpoint": "https://hooks.slack.com/services/${SLACK_WEBHOOK_ID}"
    },
    "backup": {
        "compression": {
            "enabled": true,
            "format": "gzip",
            "level": "medium"
        },
        "retention": {
            "days": 30,
            "maxBackups": 10
        },
        "schedule": {
            "enabled": true,
            "cron": "0 0 * * *"
        }
    },
    "security": {
        "encryption": {
            "enabled": true,
            "algorithm": "AES-256-GCM",
            "keyPath": "${ENCRYPTION_KEY_PATH}"
        }
    }
}
```

### Configuration Options

#### Database Settings
- `type`: Database type (mysql, postgresql, mongodb, sqlite)
- `host`: Database host
- `