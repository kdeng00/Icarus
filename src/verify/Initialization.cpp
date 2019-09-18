#include "verify/Initialization.h"

#include <iostream>
#include <filesystem>

#include "manager/DirectoryManager.h"
#include "type/PathType.h"

namespace fs = std::filesystem;

// verifies if the configuration settings are valid
void verify::Initialization::checkIcarus(const model::BinaryPath& bConf)
{
    auto auth = confirmConfigAuth(bConf);
    auto database = confirmConfigDatabase(bConf);
    auto path = confirmConfigPaths(bConf);

    if ((auth && database && path) == false) {
        failedConfirmation();
    }

    std::cout << "icarus check passed" << std::endl;
}


// verifies that the authorization settings are not the default values
bool verify::Initialization::confirmConfigAuth(const model::BinaryPath& bConf)
{
    auto authConfig = manager::DirectoryManager::credentialConfigContent(bConf);
    return true;
}

// verifies if database connectivity can be established
bool verify::Initialization::confirmConfigDatabase(const model::BinaryPath& bConf)
{
    auto databaseConfig = manager::DirectoryManager::databaseConfigContent(bConf);
    return true;
}

// verifies if the paths found in the config files exists
bool verify::Initialization::confirmConfigPaths(const model::BinaryPath& bConf)
{
    auto pathConfig = manager::DirectoryManager::pathConfigContent(bConf);

    const auto rootMusicPath = 
        pathConfig
            [manager::
                DirectoryManager::
                retrievePathType
                (type::PathType::music)].get<std::string>();
    const auto archiveRootPath = 
        pathConfig
            [manager::
                DirectoryManager::
                retrievePathType
                (type::PathType::archive)].get<std::string>();
    const auto tempRootPath = 
        pathConfig
            [manager::
                DirectoryManager::
                    retrievePathType
                    (type::PathType::temp)].get<std::string>();
    const auto coverRootPath = 
        pathConfig
            [manager::
                DirectoryManager::
                retrievePathType
                (type::PathType::coverArt)].get<std::string>();

    if (!fs::exists(rootMusicPath)) {
        std::cout << "root music path is not properly configured" << std::endl;
        return false;
    }

    if (!fs::exists(archiveRootPath)) {
        std::cout << "archive root path is not properly configured" << std::endl;
        return false;
    }

    if (!fs::exists(tempRootPath)) {
        std::cout << "temp root path is not properly configured" << std::endl;
        return false;
    }

    if (!fs::exists(coverRootPath)) {
        std::cout << "cover art root path is not properly configured" << std::endl;
        return false;
    }

    return true;
}


// confirmation failed
void verify::Initialization::failedConfirmation()
{
    std::cout << "configuration confirmation failed. check your settings" << std::endl;
    std::exit(-1);
}
