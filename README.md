# Database Backup CLI

A powerful and flexible command-line tool for backing up and restoring various types of databases. Written in modern C++, it supports multiple database systems, compression, and both local and cloud storage options.

## Quick Start

### Installation

#### macOS
```bash
# Install using Homebrew
brew tap monroestephenson/database-backup
brew install database-backup
```

### Post-Installation Setup

After installation, follow these steps to complete the setup:

1. Create your configuration directory:
```bash
mkdir -p ~/.config/db-backup
```

2. Create configuration files for your databases. Here are examples for different database types:

MySQL Configuration (`~/.config/db-backup/mysql_config.json`):
```json
{
    "database": {
        "type": "mysql",
        "host": "localhost",
        "port": 3306,
        "username": "your_username",
        "database": "your_database"
    },
    "storage": {
        "localPath": "./backups",
        "cloudProvider": "local"
    },
    "logging": {
        "logPath": "./logs",
        "logLevel": "debug"
    },
    "backup": {
        "compression": {
            "enabled": true,
            "format": "gzip",
            "level": "medium"
        }
    }
}
```

SQLite Configuration (`~/.config/db-backup/sqlite_config.json`):
```json
{
    "database": {
        "type": "sqlite",
        "database": "/path/to/your/database.db"
    },
    "storage": {
        "localPath": "./backups",
        "cloudProvider": "local"
    },
    "logging": {
        "logPath": "./logs",
        "logLevel": "debug"
    },
    "backup": {
        "compression": {
            "enabled": true,
            "format": "gzip",
            "level": "medium"
        }
    }
}
```

PostgreSQL Configuration (`~/.config/db-backup/postgres_config.json`):
```json
{
    "database": {
        "type": "postgresql",
        "host": "localhost",
        "port": 5432,
        "username": "your_username",
        "database": "your_database"
    },
    "storage": {
        "localPath": "./backups",
        "cloudProvider": "local"
    },
    "logging": {
        "logPath": "./logs",
        "logLevel": "debug"
    },
    "backup": {
        "compression": {
            "enabled": true,
            "format": "gzip",
            "level": "medium"
        }
    }
}
```

3. Create directories for backups and logs in your working directory:
```bash
mkdir -p backups logs
```

### Basic Usage

The basic command format is:
```bash
database-backup <command> [options]
```

#### Backup Commands

For MySQL:
```bash
database-backup backup --type full --db-type mysql --db-name your_database --db-host localhost --db-port 3306 --db-user your_username --config ~/.config/db-backup/mysql_config.json
```

For SQLite:
```bash
database-backup backup --type full --db-type sqlite --db-file /path/to/your/database.db --config ~/.config/db-backup/sqlite_config.json
```

For PostgreSQL:
```bash
database-backup backup --type full --db-type postgres --db-name your_database --db-host localhost --db-port 5432 --db-user your_username --config ~/.config/db-backup/postgres_config.json
```

#### Other Commands
```bash
# Show help
database-backup --help

# List available backups
database-backup list

# Restore from a backup
database-backup restore --file backups/backup_20240222_123456_full.dump.gz

# Verify a backup
database-backup verify --file backups/backup_20240222_123456_full.dump.gz
```

### Features

- ✅ **Multiple Database Support**
  - MySQL
  - PostgreSQL
  - SQLite
- 🔄 **Backup Types**
  - Full backups
- 🗜️ **Compression**
  - Gzip compression with configurable levels
- 📦 **Storage Options**
  - Local storage
- 📝 **Logging**
  - Debug and info level logging
  - Log file output

### Backup File Locations

- Backup files are stored in the `./backups` directory by default
- Logs are stored in the `./logs` directory
- Backup files are automatically compressed with gzip
- Backup filenames include timestamp and backup type: `backup_YYYYMMDD_HHMMSS_full.dump.gz`

### Troubleshooting

Common issues and solutions:

1. **"Database type is required"**
   - Make sure to specify `--db-type` in your command (mysql, postgres, or sqlite)

2. **"Database name is required"**
   - For MySQL/PostgreSQL: Include `--db-name` in your command
   - For SQLite: Use `--db-file` instead

3. **"Configuration file not found"**
   - Ensure your config file exists at the specified path
   - Check file permissions

4. **"Missing database host"**
   - For MySQL/PostgreSQL: Include `--db-host` in your command
   - Not required for SQLite

### License

MIT License - see LICENSE file for details