#include "verify/Initialization.h"

#include <iostream>
#include <filesystem>

#include "database/BaseRepository.h"
#include "manager/DirectoryManager.h"
#include "manager/TokenManager.h"
#include "type/PathType.h"

namespace fs = std::filesystem;

namespace verify {

bool Initialization::skipVerification(const std::string& argument)
{
    return argument.compare("--noverify") == 0;
}


// verifies if the configuration settings are valid
void Initialization::checkIcarus(const model::BinaryPath& bConf)
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
bool Initialization::confirmConfigAuth(const model::BinaryPath& bConf)
{
    manager::TokenManager tokMgr;

    return tokMgr.testAuth(bConf);
}

// verifies if database connectivity can be established
bool Initialization::confirmConfigDatabase(const model::BinaryPath& bConf)
{
    database::BaseRepository baseRepo(bConf);

    return baseRepo.testConnection();
}

// verifies if the paths found in the config files exists
bool Initialization::confirmConfigPaths(const model::BinaryPath& bConf)
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
void Initialization::failedConfirmation()
{
    std::cout << "configuration confirmation failed. check your settings" << std::endl;
    std::exit(-1);
}

}
