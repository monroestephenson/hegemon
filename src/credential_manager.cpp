#include "credential_manager.hpp"
#include "error/ErrorUtils.hpp"
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <cstdlib>

#ifdef __APPLE__
#include <Security/Security.h>
#endif

using namespace dbbackup;
using namespace dbbackup::error;

// Private implementation to hide sensitive data
struct CredentialManager::Impl {
    std::unordered_map<std::string, Credential> credentials;
    std::mutex credentialMutex;
    SecurityConfig config;
};

CredentialManager::~CredentialManager() {
    clearAllCredentials();
}

CredentialManager& CredentialManager::getInstance() {
    static CredentialManager instance;
    if (!instance.pImpl) {
        instance.pImpl = std::make_unique<Impl>();
    }
    return instance;
}

void CredentialManager::configure(const SecurityConfig& config) {
    if (!pImpl) {
        pImpl = std::make_unique<Impl>();
    }
    pImpl->config = config;
}

std::optional<Credential> CredentialManager::getCredential(
    const std::string& key,
    CredentialType type,
    const std::vector<CredentialSource>& preferredSources
) {
    DB_TRY_CATCH_LOG("CredentialManager", {
        if (!pImpl) {
            pImpl = std::make_unique<Impl>();
        }

        // First check if we have it in temporary storage
        {
            std::lock_guard<std::mutex> lock(pImpl->credentialMutex);
            auto it = pImpl->credentials.find(key);
            if (it != pImpl->credentials.end()) {
                auto cred = it->second;
                if (cred.temporary) {
                    pImpl->credentials.erase(it);
                }
                return cred;
            }
        }

        // Try each preferred source in order
        std::vector<CredentialSource> sources = preferredSources;
        if (sources.empty()) {
            // Default order if none specified
            sources = {
                CredentialSource::KeyStore,
                CredentialSource::File,
                CredentialSource::Environment,
                CredentialSource::SSM,
                CredentialSource::Vault,
                CredentialSource::ConfigFile
            };
        }

        for (const auto& source : sources) {
            std::optional<Credential> cred;
            switch (source) {
                case CredentialSource::Environment:
                    cred = getFromEnvironment(key);
                    break;
                case CredentialSource::File:
                    cred = getFromFile(key);
                    break;
                case CredentialSource::KeyStore:
                    cred = getFromKeyStore(key);
                    break;
                case CredentialSource::ConfigFile:
                    cred = getFromConfigFile(key);
                    break;
                case CredentialSource::SSM:
                    cred = getFromSSM(key);
                    break;
                case CredentialSource::Vault:
                    cred = getFromVault(key);
                    break;
            }
            if (cred) {
                return cred;
            }
        }

        return std::nullopt;
    });

    return std::nullopt;
}

bool CredentialManager::storeCredential(
    const std::string& key,
    const std::string& value,
    CredentialType type,
    CredentialSource source,
    bool temporary
) {
    DB_TRY_CATCH_LOG("CredentialManager", {
        if (!pImpl) {
            pImpl = std::make_unique<Impl>();
        }

        std::lock_guard<std::mutex> lock(pImpl->credentialMutex);
        pImpl->credentials[key] = Credential{value, type, source, temporary};
        return true;
    });

    return false;
}

void CredentialManager::clearCredential(const std::string& key) {
    if (!pImpl) return;

    std::lock_guard<std::mutex> lock(pImpl->credentialMutex);
    pImpl->credentials.erase(key);
}

void CredentialManager::clearAllCredentials() {
    if (!pImpl) return;

    std::lock_guard<std::mutex> lock(pImpl->credentialMutex);
    pImpl->credentials.clear();
}

std::optional<Credential> CredentialManager::getFromEnvironment(const std::string& key) {
    DB_TRY_CATCH_LOG("CredentialManager", {
        // First try with HEGEMON_ prefix
        std::string prefixedKey = "HEGEMON_" + key;
        const char* value = std::getenv(prefixedKey.c_str());
        
        // If not found, try original key
        if (!value) {
            value = std::getenv(key.c_str());
        }
        
        if (value) {
            return Credential{
                value,
                CredentialType::Password,
                CredentialSource::Environment,
                true
            };
        }
        return std::nullopt;
    });

    return std::nullopt;
}

std::optional<Credential> CredentialManager::getFromFile(const std::string& key) {
    DB_TRY_CATCH_LOG("CredentialManager", {
        // Check common password file locations with proper permissions
        std::vector<std::string> possiblePaths = {
            std::string(getenv("HOME")) + "/.config/hegemon/credentials/" + key,
            "/etc/hegemon/credentials/" + key,
            std::string(getenv("HOME")) + "/.hegemon/" + key
        };

        for (const auto& path : possiblePaths) {
            if (std::filesystem::exists(path)) {
                // Check file permissions
                auto perms = std::filesystem::status(path).permissions();
                if ((perms & std::filesystem::perms::others_read) != std::filesystem::perms::none ||
                    (perms & std::filesystem::perms::others_write) != std::filesystem::perms::none) {
                    continue; // Skip files with unsafe permissions
                }

                std::ifstream file(path);
                if (file) {
                    std::string value;
                    std::getline(file, value);
                    if (!value.empty()) {
                        return Credential{
                            value,
                            CredentialType::Password,
                            CredentialSource::File,
                            true
                        };
                    }
                }
            }
        }

        return std::nullopt;
    });

    return std::nullopt;
}

std::optional<Credential> CredentialManager::getFromKeyStore(const std::string& key) {
    DB_TRY_CATCH_LOG("CredentialManager", {
#ifdef __APPLE__
        // macOS Keychain implementation
        void* passwordData = nullptr;
        UInt32 passwordLength = 0;
        const char* service = "hegemon";
        
        OSStatus status = SecKeychainFindGenericPassword(
            nullptr,                    // default keychain
            strlen(service),           // service name length
            service,                   // service name
            key.length(),             // account name length
            key.c_str(),              // account name
            &passwordLength,          // password length
            &passwordData,            // password data
            nullptr                    // keychain item reference
        );

        if (status == errSecSuccess && passwordData) {
            std::string value(static_cast<char*>(passwordData), passwordLength);
            SecKeychainItemFreeContent(nullptr, passwordData);
            return Credential{
                value,
                CredentialType::Password,
                CredentialSource::KeyStore,
                true
            };
        }
#endif
        return std::nullopt;
    });

    return std::nullopt;
}

std::optional<Credential> CredentialManager::getFromConfigFile(const std::string& key) {
    // This is the least secure method and should be discouraged
    return std::nullopt;
}

std::optional<Credential> CredentialManager::getFromSSM(const std::string& key) {
    // TODO: Implement AWS Systems Manager Parameter Store integration
    return std::nullopt;
}

std::optional<Credential> CredentialManager::getFromVault(const std::string& key) {
    // TODO: Implement HashiCorp Vault integration
    return std::nullopt;
} 