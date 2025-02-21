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
brew install mysql-connector-c++  # For MySQL support
brew install libpq               # For PostgreSQL support
brew install mongo-cxx-driver    # For MongoDB support
brew install sqlite3            # For SQLite support
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

### Post-Installation Setup

After installation, follow these steps to complete the setup:

1. Create your configuration directory:
```bash
mkdir -p ~/.config/db-backup
```

2. Copy the configuration template:
```bash
cp /usr/local/etc/database_backup/config.template.json ~/.config/db-backup/config.json
```

3. Set up your environment variables by adding these lines to your `~/.zshrc` or `~/.bash_profile`:
```bash
# Required variables
export DB_USER=your_database_user
export DB_PASSWORD=your_database_password

# Optional variables
export SLACK_WEBHOOK_ID=your_webhook_id          # For Slack notifications
export ENCRYPTION_KEY_PATH=/path/to/your/key     # For encrypted backups
```

4. Edit your configuration file:
```bash
nano ~/.config/db-backup/config.json
```

5. Test the installation:
```bash
db-backup-cli --help
```

### Basic Usage

```bash
# Perform a full backup
db-backup-cli backup --type full

# List available backups
db-backup-cli list

# Restore from a backup
db-backup-cli restore backup_20240221_123456.dump.gz

# Start scheduled backups (based on cron settings in config)
db-backup-cli schedule

# Verify backup integrity
db-backup-cli verify backup_20240221_123456.dump.gz
```

## Configuration

The configuration file at `~/.config/db-backup/config.json` supports the following options:

```json
{
    "database": {
        "type": "mysql",          // mysql, postgresql, mongodb, sqlite
        "host": "localhost",
        "port": 3306,
        "username": "${DB_USER}",
        "password": "${DB_PASSWORD}",
        "database": "mydb"
    },
    "storage": {
        "localPath": "./backups",
        "cloudProvider": "aws",    // aws, gcp (planned), azure (planned)
        "cloudPath": "my-backup-bucket/database-backups"
    },
    "logging": {
        "logPath": "./logs/backup.log",
        "logLevel": "info",       // debug, info, warn, error
        "enableNotifications": true,
        "notificationEndpoint": "https://hooks.slack.com/services/${SLACK_WEBHOOK_ID}"
    },
    "backup": {
        "compression": {
            "enabled": true,
            "format": "gzip",     // gzip, bzip2, xz
            "level": "medium"     // low, medium, high
        },
        "retention": {
            "days": 30,
            "maxBackups": 10
        },
        "schedule": {
            "enabled": true,
        "cron": "0 0 * * *"      // Daily at midnight
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

## Troubleshooting

### Common Issues

1. **Permission Denied**
   ```
   mkdir: /Users/username/.config/db-backup: Operation not permitted
   ```
   Solution: Create the directory manually using the commands in the setup instructions.

2. **Configuration Not Found**
   ```
   Error: Configuration file not found at ~/.config/db-backup/config.json
   ```
   Solution: Follow the post-installation setup steps to create and configure your config file.

3. **Environment Variables Not Set**
   ```
   Error: Environment variable DB_USER not set
   ```
   Solution: Make sure to set the required environment variables in your shell configuration file and reload it:
   ```bash
   source ~/.zshrc  # or source ~/.bash_profile
   ```

### Getting Help

- Run `db-backup-cli --help` for command usage
- Check the logs at `./logs/backup.log`
- File issues on GitHub for bug reports or feature requests

## License

MIT License - see LICENSE file for details