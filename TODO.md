# TODO List

## High Priority
- [x] Implement proper error handling throughout the codebase
- [x] Complete database implementations
  - [x] Add MySQL connection implementation
  - [x] Add PostgreSQL connection implementation
  - [ ] Add MongoDB connection implementation (Optional - not currently advertised)
  - [x] Add SQLite connection implementation
- [x] Add configuration file support
  - [x] Create sample config.json
  - [x] Implement config file validation
  - [ ] Fix config file handling to better use values from config instead of requiring command-line arguments
  - [ ] Add password support from environment variables or config file

## Medium Priority
- [x] Implement compression functionality
  - [x] Add zlib integration
  - [x] Add compression level options
  - [ ] Add support for additional compression formats (bzip2, xz)
- [ ] Improve storage backends
  - [x] Complete local storage implementation
  - [ ] Add backup file cleanup/rotation
  - [ ] Add AWS S3 support
  - [ ] Add Google Cloud Storage support
- [x] Enhance logging system
  - [x] Add log levels configuration
  - [x] Add log file path configuration
  - [ ] Add log rotation
  - [ ] Improve error messages clarity

## Command Line Interface
- [ ] Improve CLI usability
  - [ ] Allow config file to provide defaults without requiring command-line arguments
  - [ ] Add short-form options (e.g., -t for --type)
  - [ ] Add interactive mode for configuration
  - [ ] Add command to generate config file templates
- [ ] Add backup management commands
  - [ ] Implement 'list' command functionality
  - [ ] Add 'prune' command for old backups
  - [ ] Add 'verify' command implementation

## Testing
- [ ] Add more unit tests
  - [x] Config parser tests
  - [x] Compression tests
  - [ ] Storage backend tests
  - [ ] Database connection tests
  - [ ] Add tests for MySQL support
  - [ ] Add tests for SQLite support
- [ ] Add integration tests
  - [ ] Test full backup workflow
  - [ ] Test restore workflow
  - [ ] Test compression
- [ ] Add end-to-end tests
- [ ] Add performance tests

## Documentation
- [x] Update README with current features
- [x] Add configuration guide
- [ ] Add detailed troubleshooting guide
  - [ ] Document MySQL-specific issues
  - [ ] Document SQLite-specific issues
  - [ ] Document PostgreSQL-specific issues
- [ ] Add development guide
- [ ] Add API documentation for contributors

## Features
- [x] Implement full backup support
- [ ] Implement incremental backup support
- [ ] Implement differential backup support
- [ ] Add backup verification
- [ ] Add backup encryption
- [ ] Improve backup retention policies
  - [ ] Implement automatic cleanup
  - [ ] Add flexible retention rules
- [ ] Add notification system
  - [ ] Add Slack notifications
  - [ ] Add email notifications
- [x] Add progress indication for operations

## CI/CD
- [ ] Set up GitHub Actions
  - [ ] Add build workflow
  - [ ] Add test workflow
  - [ ] Add code coverage
  - [ ] Add static analysis
- [ ] Add release automation
  - [ ] Automate version bumping
  - [ ] Automate Homebrew formula updates
- [ ] Add Docker support
  - [ ] Create Dockerfile
  - [ ] Create docker-compose.yml for testing

## Immediate Priorities (Based on Testing)
1. Fix config file usage to reduce required command-line arguments
2. Implement proper password handling
3. Complete 'list' and 'verify' commands
4. Add backup file rotation/cleanup
5. Improve error messages and validation
6. Add tests for MySQL and SQLite implementations
7. Document common issues and solutions

## Known Issues to Fix
1. Command-line arguments shouldn't be required when config file has the information
2. Password handling needs to be more secure
3. Backup file cleanup is not implemented
4. Error messages could be more user-friendly
5. List and verify commands are not implemented
6. Need better validation for SQLite database paths
7. Progress indication could be improved



The CLI format changes we made haven't been included in the latest release
The tool creates duplicate backup files with slightly different names
List and verify commands are not implemented yet
Some environment variables are required even when features are disabled