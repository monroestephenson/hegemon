# Database Backup CLI

A powerful and flexible command-line tool for backing up and restoring various types of databases. Written in modern C++, it supports multiple database systems, compression, and both local and cloud storage options.

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
- `port`: Database port
- `username`: Database username (supports env vars)
- `password`: Database password (supports env vars)
- `database`: Database name

#### Storage Settings
- `localPath`: Local backup storage path
- `cloudProvider`: Cloud storage provider (aws, gcp, azure)
- `cloudPath`: Cloud storage path/bucket

#### Logging Settings
- `logPath`: Path to log file
- `logLevel`: Log level (debug, info, warn, error)
- `enableNotifications`: Enable Slack notifications
- `notificationEndpoint`: Slack webhook URL (supports env vars)

#### Backup Settings
- Compression:
  - `enabled`: Enable compression
  - `format`: Compression format (gzip, bzip2, xz)
  - `level`: Compression level (low, medium, high)
- Retention:
  - `days`: Days to keep backups
  - `maxBackups`: Maximum number of backups to keep
- Schedule:
  - `enabled`: Enable scheduled backups
  - `cron`: Cron expression for schedule

#### Security Settings
- Encryption:
  - `enabled`: Enable encryption
  - `algorithm`: Encryption algorithm
  - `keyPath`: Path to encryption key (supports env vars)

## Building

```bash
# Clone the repository
git clone https://github.com/yourusername/database_backup.git
cd database_backup

# Create build directory
mkdir build && cd build

# Configure and build
cmake ..
make

# Run tests
make test
```

## Usage

### Basic Commands

```bash
# Set required environment variables
source .env  # If using a .env file

# Perform a full backup
./my_db_backup_cli backup --type full --config config.json

# Restore from backup
./my_db_backup_cli restore --file backup_20240221_123456.dump.gz

# Start scheduled backups
./my_db_backup_cli schedule --config config.json
```

### Example .env File

Create a `.env` file (don't commit to version control):

```bash
# Database credentials
export DB_USER=myuser
export DB_PASSWORD=mypassword

# Slack notifications
export SLACK_WEBHOOK_ID=T00000000/B00000000/XXXXXXXXXXXXXXXXXXXXXXXX

# Encryption
export ENCRYPTION_KEY_PATH=/path/to/encryption.key
```

Then source it before running the application:
```bash
source .env
```json
{
  "database": {
    "type": "mysql",
    "host": "localhost",
    "port": 3306,
    "username": "user",
    "password": "password",
    "database": "mydb"
  },
  "storage": {
    "type": "local",
    "localPath": "/path/to/backups"
  },
  "compression": {
    "enabled": true,
    "level": "medium"
  },
  "logging": {
    "level": "info",
    "file": "backup.log"
  }
}
```

## Development

### Project Structure

```
.
├── src/                    # Source files
│   ├── cli/               # CLI implementation
│   ├── db/                # Database connectors
│   ├── backup/            # Backup implementation
│   └── utils/             # Utility functions
├── include/               # Header files
├── tests/                 # Test files
├── external/              # External dependencies
└── examples/             # Example configurations
```

### Adding New Features

1. Fork the repository
2. Create a feature branch
3. Add tests for your feature
4. Implement your feature
5. Submit a pull request

## Testing

```bash
# Run all tests
make test

# Run specific test suite
./unit_tests --gtest_filter=BackupManagerTest.*
```

## Contributing

Contributions are welcome! Please read our [Contributing Guide](CONTRIBUTING.md) for details on our code of conduct and the process for submitting pull requests.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- [Google Test](https://github.com/google/googletest) for testing framework
- [spdlog](https://github.com/gabime/spdlog) for logging
- [CLI11](https://github.com/CLIUtils/CLI11) for command line parsing
- [nlohmann/json](https://github.com/nlohmann/json) for JSON handling

## Support

For support, please:
1. Check the [documentation](docs/)
2. Search existing [issues](https://github.com/yourusername/database_backup/issues)
3. Create a new issue if needed

## Roadmap

See our [TODO.md](TODO.md) file for planned features and improvements. 