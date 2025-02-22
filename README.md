# Hegemon - Database Management CLI

A powerful and flexible command-line tool for managing and backing up various types of databases. Written in modern C++, it supports multiple database systems, compression, and both local and cloud storage options.

## Quick Start

### Installation

#### macOS
```bash
# Install using Homebrew
brew tap monroestephenson/database-backup
brew install hegemon
```

### Post-Installation Setup

After installation, follow these steps to complete the setup:

1. Create your configuration directory:
```bash
mkdir -p ~/.config/hegemon
```

2. Create configuration files for your databases. Here are examples for different database types:

MySQL Configuration (`~/.config/hegemon/mysql_config.json`):
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

SQLite Configuration (`~/.config/hegemon/sqlite_config.json`):
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

PostgreSQL Configuration (`~/.config/hegemon/postgres_config.json`):
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
hegemon <command> [database] [options]
```

#### Backup Commands

Simple backup commands (uses default config files):
```bash
# MySQL backup
hegemon backup mysql

# PostgreSQL backup
hegemon backup postgres

# SQLite backup
hegemon backup sqlite
```

Backup commands with options:
```bash
# MySQL backup with custom database
hegemon backup mysql -n mydatabase -u myuser

# PostgreSQL backup with custom host
hegemon backup postgres -h db.example.com -n mydb

# SQLite backup with specific file
hegemon backup sqlite -f /path/to/database.db
```

#### Other Commands
```bash
# Show help
hegemon help

# List available backups
hegemon list

# Restore from a backup
hegemon restore backup_20240222.dump.gz

# Verify a backup
hegemon verify backup_20240222.dump.gz
```

### Command Options

- `-c, --config <path>`    Custom config file path
- `-t, --type <type>`      Backup type (full|incremental)
- `-h, --host <host>`      Database host
- `-p, --port <port>`      Database port
- `-n, --name <dbname>`    Database name
- `-u, --user <user>`      Database username
- `-f, --file <path>`      SQLite database file path
- `--verbose`              Enable verbose output
- `--help`                 Show help message

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

### Configuration

By default, Hegemon looks for configuration files in the following locations:
- MySQL: `~/.config/hegemon/mysql_config.json`
- PostgreSQL: `~/.config/hegemon/postgres_config.json`
- SQLite: `~/.config/hegemon/sqlite_config.json`

You can override these defaults using the `-c` option to specify a custom config file.

### Troubleshooting

Common issues and solutions:

1. **"Database type required"**
   - Make sure to specify the database type after the backup command (mysql, postgres, or sqlite)
   - Example: `hegemon backup mysql`

2. **"Database name is required"**
   - For MySQL/PostgreSQL: Include `-n` option
   - For SQLite: Use `-f` option
   - Or set these in your config file

3. **"Configuration file not found"**
   - Create the appropriate config file in `~/.config/hegemon/`
   - Or specify a custom config file with `-c`

4. **"Missing database host"**
   - Set the host in your config file
   - Or use `-h` option
   - Not required for SQLite

### License

MIT License - see LICENSE file for details