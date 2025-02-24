# Hegemon - Advanced Database Backup Management Tool

A powerful and secure command-line tool for managing database backups, written in modern C++. Hegemon supports multiple database systems, compression, encryption, and both local and cloud storage options.

## Prerequisites

### System Requirements
- Operating System: macOS, Linux, or Windows WSL2
- Disk Space: At least 2x the size of your largest database
- Memory: Minimum 512MB free RAM
- Terminal: Bash or Zsh shell

### Required Software
- For MySQL backups: MySQL Server 5.7+ and MySQL client tools
- For PostgreSQL backups: PostgreSQL 10+ and psql client tools
- For SQLite backups: SQLite 3.x+

### Required Permissions
- Write access to `~/.config/hegemon/`
- Write access to your chosen backup directory
- Database administrator access for initial setup
- Read access to the database being backed up

## Installation

### Using Homebrew (macOS)
```bash
# Install Hegemon
brew tap monroestephenson/database-backup
brew install hegemon

# Verify installation
hegemon --version  # Should show version number
```

### Post-Installation Setup

1. Create the configuration directory:
```bash
# Create directory
mkdir -p ~/.config/hegemon

# Verify directory creation
ls -la ~/.config/hegemon  # Should show empty directory
```

2. Create a configuration file for your database (examples below in Database-Specific Setup).

## Database-Specific Setup

### PostgreSQL Setup

1. Create the configuration file:
```bash
# Create config file - REPLACE your_username and your_database with your actual values
cat > ~/.config/hegemon/config.json << 'EOF'
{
    "database": {
        "type": "postgresql",
        "host": "localhost",
        "port": 5432,
        "username": "your_username",     # Example: "jsmith"
        "database": "your_database"      # Example: "company_data"
    },
    "storage": {
        "localPath": "./backups",        # Example: "/Users/jsmith/backups"
        "cloudProvider": "local"
    },
    "logging": {
        "logPath": "./logs",             # Example: "/Users/jsmith/logs"
        "logLevel": "debug",
        "enableNotifications": false
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
    },
    "security": {
        "encryption": {
            "enabled": false
        }
    }
}
EOF

# Verify config file creation
cat ~/.config/hegemon/config.json  # Should show the config you just created
```

2. Set up PostgreSQL authentication (MUST BE DONE BEFORE STEP 3):
   ```bash
   # Create or update ~/.pgpass - REPLACE your_password with actual password
   echo "localhost:5432:your_database:your_username:your_password" > ~/.pgpass
   chmod 600 ~/.pgpass

   # Verify .pgpass permissions
   ls -l ~/.pgpass  # Should show: -rw-------
   ```

3. Create a test database and table:
   ```bash
   # Create database
   createdb your_database
   
   # Verify database creation
   psql -l | grep your_database  # Should show your database

   # Create test table and data
   psql your_database -c "CREATE TABLE test (id SERIAL PRIMARY KEY, name TEXT);"
   psql your_database -c "INSERT INTO test (name) VALUES ('test1'), ('test2');"
   
   # Verify data
   psql your_database -c "SELECT * FROM test;"  # Should show 2 rows
   ```

4. Test backup:
   ```bash
   # Create backup
   hegemon backup postgres -c ~/.config/hegemon/config.json --verbose
   
   # Expected successful output:
   # [INFO] Starting backup of postgresql database your_database
   # [INFO] Backup completed successfully
   # [INFO] Backup saved to: ./backups/backup_YYYYMMDD_HHMMSS_full.dump.gz
   
   # Verify backup file exists
   ls -l ./backups/  # Should show .dump.gz file
   ```

5. Test restore (to a new database):
   ```bash
   # Create new database for restore testing
   createdb your_database_restore
   
   # Verify new database
   psql -l | grep your_database_restore  # Should show new database
   
   # Update config to point to new database
   sed -i '' 's/your_database/your_database_restore/g' ~/.config/hegemon/config.json
   
   # Verify config change
   grep your_database_restore ~/.config/hegemon/config.json  # Should show updated database name
   
   # Restore the backup (replace YYYYMMDD_HHMMSS with actual timestamp)
   hegemon restore backups/backup_YYYYMMDD_HHMMSS_full.dump.gz -c ~/.config/hegemon/config.json
   
   # Verify restored data
   psql your_database_restore -c "SELECT * FROM test;"  # Should show same 2 rows
   ```

### MySQL Setup

1. Create the configuration file:
```bash
# Create config file - REPLACE your_username and your_password with actual values
cat > ~/.config/hegemon/config.json << 'EOF'
{
    "database": {
        "type": "mysql",
        "host": "localhost",
        "port": 3306,
        "username": "your_username",     # Example: "jsmith"
        "password": "your_password",     # Example: "mysecurepass123"
        "database": "your_database"      # Example: "company_data"
    },
    "storage": {
        "localPath": "./backups",        # Example: "/Users/jsmith/backups"
        "cloudProvider": "local"
    },
    "logging": {
        "logPath": "./logs",             # Example: "/Users/jsmith/logs"
        "logLevel": "debug",
        "enableNotifications": false
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
    },
    "security": {
        "encryption": {
            "enabled": false
        }
    }
}
EOF

# Verify config file creation
cat ~/.config/hegemon/config.json  # Should show the config you just created
```

2. Set up MySQL user and privileges (MUST BE DONE BEFORE STEP 3):
   ```bash
   # Connect as root to create user and grant privileges
   # REPLACE your_username, your_password with actual values
   mysql -u root -e "CREATE USER IF NOT EXISTS 'your_username'@'localhost' IDENTIFIED BY 'your_password';"
   mysql -u root -e "GRANT PROCESS, RELOAD, LOCK TABLES, SELECT ON *.* TO 'your_username'@'localhost';"
   mysql -u root -e "GRANT ALL PRIVILEGES ON your_database.* TO 'your_username'@'localhost';"
   mysql -u root -e "FLUSH PRIVILEGES;"
   
   # Verify user creation and privileges
   mysql -u root -e "SHOW GRANTS FOR 'your_username'@'localhost';"  # Should show granted privileges
   ```

3. Create a test database and table:
   ```bash
   # Create database and table
   mysql -u root -e "CREATE DATABASE IF NOT EXISTS your_database;"
   mysql -u your_username -p'your_password' your_database -e "CREATE TABLE test (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255));"
   mysql -u your_username -p'your_password' your_database -e "INSERT INTO test (name) VALUES ('test1'), ('test2');"
   
   # Verify data
   mysql -u your_username -p'your_password' your_database -e "SELECT * FROM test;"  # Should show 2 rows
   ```

4. Test backup:
   ```bash
   # Create backup
   hegemon backup mysql -c ~/.config/hegemon/config.json --verbose
   
   # Expected successful output:
   # [INFO] Starting backup of mysql database your_database
   # [INFO] Backup completed successfully
   # [INFO] Backup saved to: ./backups/backup_YYYYMMDD_HHMMSS_full.dump.gz
   
   # Verify backup file exists
   ls -l ./backups/  # Should show .dump.gz file
   ```

5. Test restore (to a new database):
   ```bash
   # Create new database for restore testing
   mysql -u root -e "CREATE DATABASE your_database_restore;"
   mysql -u root -e "GRANT ALL PRIVILEGES ON your_database_restore.* TO 'your_username'@'localhost';"
   mysql -u root -e "FLUSH PRIVILEGES;"
   
   # Update config to point to new database
   cp ~/.config/hegemon/config.json ~/.config/hegemon/restore_config.json
   sed -i '' 's/your_database/your_database_restore/g' ~/.config/hegemon/restore_config.json
   
   # Restore the backup (replace YYYYMMDD_HHMMSS with actual timestamp)
   hegemon restore backups/backup_YYYYMMDD_HHMMSS_full.dump.gz -c ~/.config/hegemon/restore_config.json
   
   # Verify restored data
   mysql -u your_username -p'your_password' your_database_restore -e "SELECT * FROM test;"  # Should show same 2 rows
   ```

> **⚠️ Important Notes about MySQL Setup**:
> 1. The password MUST be included directly in the config file as shown above
> 2. The user needs both global privileges (PROCESS, RELOAD, LOCK TABLES, SELECT) and database-specific privileges
> 3. When restoring to a new database, make sure to:
>    - Create the new database first
>    - Grant appropriate privileges to the user on the new database
>    - Update the config file to point to the new database
> 4. If you get "Access denied" errors, verify:
>    - The password in the config file matches the MySQL user's password
>    - The user has all required privileges
>    - The database exists and the user has access to it

### SQLite Setup

1. Create the configuration file:
```bash
# Create config file - REPLACE /absolute/path/to/your/database.db with actual path
cat > ~/.config/hegemon/config.json << 'EOF'
{
    "database": {
        "type": "sqlite",
        "database": "/absolute/path/to/your/database.db"  # Example: "/Users/jsmith/data/myapp.db"
    },
    "storage": {
        "localPath": "./backups",        # Example: "/Users/jsmith/backups"
        "cloudProvider": "local"
    },
    "logging": {
        "logPath": "./logs",             # Example: "/Users/jsmith/logs"
        "logLevel": "debug",
        "enableNotifications": false
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
    },
    "security": {
        "encryption": {
            "enabled": false
        }
    }
}
EOF

# Verify config file creation
cat ~/.config/hegemon/config.json  # Should show the config you just created
```

2. Create a test database:
   ```bash
   # Create database directory if needed
   mkdir -p "$(dirname "/absolute/path/to/your/database.db")"
   
   # Create test database and data
   sqlite3 "/absolute/path/to/your/database.db" << 'EOF'
   CREATE TABLE test (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT);
   INSERT INTO test (name) VALUES ('test1'), ('test2');
   .quit
   EOF
   
   # Verify data
   sqlite3 "/absolute/path/to/your/database.db" "SELECT * FROM test;"  # Should show 2 rows
   ```

3. Test backup:
   ```bash
   # Create backup
   hegemon backup sqlite -c ~/.config/hegemon/config.json --verbose
   
   # Expected successful output:
   # [INFO] Starting backup of sqlite database
   # [INFO] Backup completed successfully
   # [INFO] Backup saved to: ./backups/backup_YYYYMMDD_HHMMSS_full.dump.gz
   
   # Verify backup file exists
   ls -l ./backups/  # Should show .dump.gz file
   ```

4. Test restore (to a new database):
   ```bash
   # Update config to point to new database
   sed -i '' 's/database.db/database_restore.db/g' ~/.config/hegemon/config.json
   
   # Verify config change
   grep database_restore.db ~/.config/hegemon/config.json  # Should show updated database path
   
   # Restore the backup (replace YYYYMMDD_HHMMSS with actual timestamp)
   hegemon restore backups/backup_YYYYMMDD_HHMMSS_full.dump.gz -c ~/.config/hegemon/config.json
   
   # Verify restored data
   sqlite3 "/absolute/path/to/your/database_restore.db" "SELECT * FROM test;"  # Should show same 2 rows
   ```

## Common Issues and Troubleshooting

### General Issues
- Error: "Permission denied"
  ```bash
  # Check directory permissions
  ls -la ~/.config/hegemon/
  ls -la ./backups/
  ls -la ./logs/
  
  # Fix permissions if needed
  chmod 700 ~/.config/hegemon/
  chmod 700 ./backups/
  chmod 700 ./logs/
  ```

- Error: "Directory not found"
  ```bash
  # Create required directories
  mkdir -p ~/.config/hegemon/
  mkdir -p ./backups/
  mkdir -p ./logs/
  ```

### PostgreSQL Issues
- Error: "role does not exist"
  ```bash
  # Create the user
  createuser your_username
  
  # Verify user creation
  psql -c "\du" | grep your_username
  ```

- Error: "database does not exist"
  ```bash
  # Create the database
  createdb your_database
  
  # Verify database creation
  psql -l | grep your_database
  ```

- Error: "permission denied"
  ```bash
  # Grant privileges
  psql -c "GRANT ALL PRIVILEGES ON DATABASE your_database TO your_username;"
  
  # Verify privileges
  psql -c "\l+" | grep your_database
  ```

### MySQL Issues
- Error: "Access denied for user"
  ```bash
  # Verify password in config matches MySQL
  mysql -u your_username -p'your_password' -e "SELECT 1;"
  
  # Reset password if needed
  mysql -u root -e "ALTER USER 'your_username'@'localhost' IDENTIFIED BY 'your_password';"
  ```

- Error: "Can't connect to MySQL server"
  ```bash
  # Check if MySQL is running
  mysql.server status
  
  # Start MySQL if needed
  mysql.server start
  ```

- Error: "PROCESS privilege" errors
  ```bash
  # Grant additional privileges
  mysql -u root -e "GRANT PROCESS, RELOAD, LOCK TABLES, SELECT ON *.* TO 'your_username'@'localhost';"
  mysql -u root -e "FLUSH PRIVILEGES;"
  ```

### SQLite Issues
- Error: "unable to open database file"
  ```bash
  # Check file existence and permissions
  ls -l "/absolute/path/to/your/database.db"
  
  # Check directory permissions
  ls -la "$(dirname "/absolute/path/to/your/database.db")"
  ```

- Error: "database is locked"
  ```bash
  # Check for other processes using the database
  lsof "/absolute/path/to/your/database.db"
  
  # Kill blocking processes if needed
  fuser -k "/absolute/path/to/your/database.db"
  ```

## Command Reference

### Checking Tool Status
```bash
# Version check
hegemon --version

# Help information
hegemon --help
```

### Backup Commands
```bash
# Basic backup (replace TYPE with mysql, postgres, or sqlite)
hegemon backup TYPE -c /path/to/config.json

# Verbose backup
hegemon backup TYPE -c /path/to/config.json --verbose

# Expected successful output:
# [INFO] Starting backup...
# [INFO] Backup completed successfully
# [INFO] Backup saved to: ./backups/backup_YYYYMMDD_HHMMSS_full.dump.gz
```

### List and Verify Commands
```bash
# List all backups
hegemon list TYPE -c /path/to/config.json

# Expected output:
# Available backups for TYPE:
# backup_20240223_120420_full.dump.gz
# backup_20240223_130420_full.dump.gz

# Verify backup integrity
hegemon verify backups/backup_YYYYMMDD_HHMMSS_full.dump.gz -c /path/to/config.json

# Expected output:
# [INFO] Verifying backup...
# [INFO] Backup integrity check passed
```

### Restore Commands
```bash
# Basic restore
hegemon restore backups/backup_YYYYMMDD_HHMMSS_full.dump.gz -c /path/to/config.json

# Verbose restore
hegemon restore backups/backup_YYYYMMDD_HHMMSS_full.dump.gz -c /path/to/config.json --verbose

# Expected successful output:
# [INFO] Starting restore...
# [INFO] Restore completed successfully
```

## Verifying Success

### After Backup
```bash
# Check backup file exists
ls -l ./backups/backup_YYYYMMDD_HHMMSS_full.dump.gz

# Verify backup size (should be non-zero)
du -h ./backups/backup_YYYYMMDD_HHMMSS_full.dump.gz

# Check backup integrity
hegemon verify backups/backup_YYYYMMDD_HHMMSS_full.dump.gz -c /path/to/config.json
```

### After Restore
```bash
# For MySQL:
mysql -u your_username -p'your_password' your_database -e "SELECT COUNT(*) FROM test;"

# For PostgreSQL:
psql your_database -c "SELECT COUNT(*) FROM test;"

# For SQLite:
sqlite3 your_database "SELECT COUNT(*) FROM test;"

# Expected output should match the count from your original database
```

## Best Practices

1. **Always Test First**
   - Test backups with a small database first
   - Verify restore process works before relying on backups
   - Keep test data for verification

2. **Security**
   - Use strong passwords
   - Restrict file permissions
   - Regular security audits

3. **Monitoring**
   - Check logs regularly
   - Set up notifications
   - Monitor disk space

4. **Maintenance**
   - Regular test restores
   - Clean old backups
   - Update configurations

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.