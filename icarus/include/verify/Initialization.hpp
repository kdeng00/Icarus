#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <utility>

#include <icarus_lib/icarus.h>

#include "database/BaseRepository.h"
#include "manager/DirectoryManager.h"
#include "manager/Manager.h"
#include "type/PathType.h"

namespace fs = std::filesystem;

namespace verify
{
    template<typename Config = icarus_lib::binary_path>
    class Initialization
    {
    public:
        static bool skipVerification(const std::string &argument) noexcept
        {
            return argument.compare("--noverify") == 0;
        }

        static void checkIcarus(const Config &bConf)
        {
            auto auth = confirmConfigAuth(bConf);
            auto database = confirmConfigDatabase(bConf);
            auto path = confirmConfigPaths(bConf);

            if ((!auth) && (!database) && (!path)) {
                failedConfirmation();
            }

            std::cout << "icarus check passed\n";
        }
    private:
        static bool confirmConfigAuth(const Config &bConf)
        {
            manager::token_manager tokMgr;

            return tokMgr.testAuth(bConf);
        }

        static bool confirmConfigDatabase(const Config &bConf)
        {
            database::BaseRepository baseRepo(bConf);

            return baseRepo.testConnection();
        }
        static bool confirmConfigPaths(const Config &bConf)
        {
            using manager::DirectoryManager;
            using namespace type;

            auto pathConfig = DirectoryManager::pathConfigContent(bConf);
            const auto musicType = PathType::music;
            const auto archiveType = PathType::archive;
            const auto tempRootType = PathType::temp;
            const auto coverArtType = PathType::coverArt;

            auto rootMusicPath = 
                pathConfig[DirectoryManager::retrievePathType(musicType)];
            auto archiveRootPath = 
                pathConfig
                [DirectoryManager::retrievePathType(archiveType)];
            auto tempRootPath = 
                pathConfig
                [DirectoryManager::retrievePathType(tempRootType)];

            auto coverRootPath = 
                pathConfig
                [DirectoryManager::retrievePathType(coverArtType)];

            std::vector<std::string> paths;
            paths.reserve(4);
            paths.emplace_back(std::move(rootMusicPath));
            paths.emplace_back(std::move(archiveRootPath));
            paths.emplace_back(std::move(tempRootPath));
            paths.emplace_back(std::move(coverRootPath));

            return confirmMultiplePaths(paths);
        }

        static void failedConfirmation()
        {
            std::cout << "configuration confirmation failed. check your settings\n";
            std::exit(-1);
        }

        template<typename Obj = std::string, typename Container = std::vector<Obj>>
        static bool confirmMultiplePaths(const Container &con)
        {
            auto result = true;

            for (auto &path : con)
            {
                if (!fs::exists(path))
                {
                    std::cout << path << " is not properly configured\n";
                    return false;
                }
            }


            return result;
        }
    };
}

#endif
