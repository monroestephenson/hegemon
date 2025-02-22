# Hegemon - Database Management CLI

A powerful and flexible command-line tool for managing and backing up various types of databases. Written in modern C++, it supports multiple database systems, compression, and both local and cloud storage options.

## Features

- ✅ **Multiple Database Support**
  - MySQL/MariaDB
  - PostgreSQL
  - SQLite
- 🔄 **Backup Types**
  - Full backups (with schema and data)
  - Compressed backups with gzip
  - Automatic backup rotation
- 🗜️ **Compression**
  - Gzip compression with configurable levels (low, medium, high)
  - Automatic handling of compression/decompression
- 📦 **Storage Options**
  - Local storage with configurable paths
  - Backup file rotation with retention policies
- 📝 **Logging & Monitoring**
  - Detailed logging with configurable levels
  - Backup verification
  - List and inspect backups
- 🔒 **Security**
  - Support for password authentication
  - Environment variable support for sensitive data
  - Optional encryption support

## Quick Start

### Installation

#### macOS
```bash
# Install using Homebrew
brew tap monroestephenson/database-backup
brew install hegemon
```

### Post-Installation Setup

1. Create your configuration directory:
```bash
mkdir -p ~/.config/hegemon
```

2. Create backup and log directories:
```bash
mkdir -p backups logs
```

3. Create configuration files for your databases (see examples below)

4. Set up environment variables (if needed):
```bash
# Add to your ~/.zshrc or ~/.bash_profile
export DB_PASSWORD="your_database_password"
```

### Database-Specific Setup

#### PostgreSQL Setup
1. Create PostgreSQL config:
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
        },
        "retention": {
            "days": 30,
            "maxBackups": 10
        }
    }
}
```

2. Ensure PostgreSQL user has proper permissions:
```sql
GRANT CONNECT ON DATABASE your_database TO your_username;
GRANT SELECT ON ALL TABLES IN SCHEMA public TO your_username;
```

#### MySQL Setup
1. Create MySQL config:
```json
{
    "database": {
        "type": "mysql",
        "host": "localhost",
        "port": 3306,
        "username": "your_username",
        "password": "${DB_PASSWORD}",
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
        },
        "retention": {
            "days": 30,
            "maxBackups": 10
        }
    }
}
```

2. Grant necessary MySQL permissions:
```sql
GRANT SELECT, SHOW VIEW, PROCESS, TRIGGER ON *.* TO 'your_username'@'localhost';
FLUSH PRIVILEGES;
```

#### SQLite Setup
1. Create SQLite config:
```json
{
    "database": {
        "type": "sqlite",
        "database": "/absolute/path/to/your/database.db"
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
        },
        "retention": {
            "days": 30,
            "maxBackups": 10
        }
    }
}
```

2. Ensure the SQLite database file is readable by your user

## Usage Guide

### Basic Commands

```bash
# Show help
hegemon --help

# List available backups
hegemon list [database]

# Create a backup
hegemon backup [database] [options]

# Restore from backup
hegemon restore <backup-file>

# Verify backup integrity
hegemon verify <backup-file>
```

### Backup Examples

```bash
# PostgreSQL backup with default config
hegemon backup postgres

# MySQL backup with custom config
hegemon backup mysql -c /path/to/custom_config.json

# SQLite backup with specific database file
hegemon backup sqlite -f /path/to/database.db

# Full backup with verbose output
hegemon backup postgres --verbose

# List all backups
hegemon list

# List PostgreSQL backups only
hegemon list postgres

# Verify a specific backup
hegemon verify backups/backup_20240222_123456_full.dump.gz

# Restore from backup
hegemon restore backups/backup_20240222_123456_full.dump.gz
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

### Backup File Format

Backup files follow this naming convention:
```
backup_YYYYMMDD_HHMMSS_type.dump[.gz]
```
Example: `backup_20240222_143022_full.dump.gz`

### Configuration File Locations

Default config file locations:
- PostgreSQL: `~/.config/hegemon/postgres_config.json`
- MySQL: `~/.config/hegemon/mysql_config.json`
- SQLite: `~/.config/hegemon/sqlite_config.json`

Override with `-c` option: `hegemon backup mysql -c /path/to/config.json`

## Troubleshooting

### Common Issues

1. **Authentication Errors**
   - Ensure database user exists and has correct permissions
   - Check password in config or environment variable
   - For MySQL: `GRANT SELECT, SHOW VIEW, PROCESS, TRIGGER`
   - For PostgreSQL: `GRANT CONNECT, SELECT`

2. **File Permission Issues**
   - Check permissions on backup and log directories
   - Ensure database files are readable (SQLite)
   - Verify config file permissions (should be 600)

3. **Configuration Issues**
   - Validate JSON syntax in config files
   - Ensure all required fields are present
   - Check environment variables are set
   - Verify database connection details

4. **Backup/Restore Failures**
   - Check disk space for backups
   - Verify database connectivity
   - Ensure proper permissions
   - Check log files for detailed errors

### Debug Tips

1. Enable verbose logging:
   ```bash
   hegemon backup postgres --verbose
   ```

2. Check log files:
   ```bash
   tail -f logs/hegemon.log
   ```

3. Verify backup integrity:
   ```bash
   hegemon verify <backup-file>
   ```

4. Test database connectivity:
   - PostgreSQL: `psql -h localhost -U username -d database`
   - MySQL: `mysql -h localhost -u username -p database`
   - SQLite: `sqlite3 database.db .tables`

## License

MIT License - see LICENSE file for details