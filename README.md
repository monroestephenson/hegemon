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
- 🗜️ **Compression**
  - Automatic backup compression
  - Multiple compression formats support
- 📦 **Storage Options**
  - Local storage
  - Cloud storage (planned)
    - AWS S3
    - Google Cloud Storage
    - Azure Blob Storage
- ⏰ **Scheduling**
  - Automated backup scheduling
  - Flexible scheduling options
- 📢 **Notifications**
  - Slack integration (planned)
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

## Building

```bash
# Clone the repository
git clone https://github.com/monroestephenson/database_backup.git
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
# Perform a full backup
./my_db_backup_cli backup --type full --config config.json

# Restore from backup
./my_db_backup_cli restore --file backup_20240221_123456.dump.gz

# Start scheduled backups
./my_db_backup_cli schedule --config config.json
```

### Configuration

Create a `config.json` file:

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
