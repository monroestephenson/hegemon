#pragma once

#include <string>
#include <memory>
#include <optional>
#include "config.hpp"

namespace dbbackup {

enum class CredentialType {
    Password,
    APIKey,
    Certificate,
    Token
};

enum class CredentialSource {
    Environment,    // Environment variables
    File,          // Password/credential file
    KeyStore,      // System keystore (e.g., Keychain on macOS)
    ConfigFile,    // Direct from config (least secure, discouraged)
    SSM,           // AWS Systems Manager Parameter Store
    Vault          // HashiCorp Vault
};

struct Credential {
    std::string value;
    CredentialType type;
    CredentialSource source;
    bool temporary = false;  // If true, credential will be cleared after use
};

class CredentialManager {
public:
    static CredentialManager& getInstance();

    // Get a credential, optionally specifying preferred sources
    std::optional<Credential> getCredential(
        const std::string& key,
        CredentialType type,
        const std::vector<CredentialSource>& preferredSources = {});

    // Store a credential (for temporary storage)
    bool storeCredential(
        const std::string& key,
        const std::string& value,
        CredentialType type,
        CredentialSource source,
        bool temporary = true);

    // Clear a stored credential
    void clearCredential(const std::string& key);

    // Clear all stored credentials
    void clearAllCredentials();

    // Configure the credential manager
    void configure(const SecurityConfig& config);

private:
    CredentialManager() = default;
    ~CredentialManager();
    
    CredentialManager(const CredentialManager&) = delete;
    CredentialManager& operator=(const CredentialManager&) = delete;

    // Helper methods for different credential sources
    std::optional<Credential> getFromEnvironment(const std::string& key);
    std::optional<Credential> getFromFile(const std::string& key);
    std::optional<Credential> getFromKeyStore(const std::string& key);
    std::optional<Credential> getFromConfigFile(const std::string& key);
    std::optional<Credential> getFromSSM(const std::string& key);
    std::optional<Credential> getFromVault(const std::string& key);

    // Secure storage for temporary credentials
    struct Impl;
    std::unique_ptr<Impl> pImpl;
}; 