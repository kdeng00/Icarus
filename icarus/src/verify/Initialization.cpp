#include "verify/Initialization.h"

#include <utility>

#include "database/Repositories.h"
#include "manager/Manager.h"
#include "type/PathType.h"

using std::cout;
using std::vector;
using std::string;

using icarus_lib::binary_path;

using manager::directory_manager;
using type::PathType;

namespace verify
{
    bool Initialization::skipVerification(const string &argument) noexcept
    {
        return argument.compare("--noverify") == 0;
    }

    void Initialization::checkIcarus(const binary_path &bConf)
    {
        auto database = confirmConfigDatabase(bConf);
        auto path = confirmConfigPaths(bConf);

        if ((!database) && (!path)) {
            failedConfirmation();
        }

        cout << "icarus check passed\n";
    }

    bool Initialization::confirmConfigDatabase(const binary_path &bConf)
    {
        database::base_repo baseRepo(bConf);

        return baseRepo.testConnection();
    }
    bool Initialization::confirmConfigPaths(const binary_path &bConf)
    {
        auto pathConfig = directory_manager::pathConfigContent(bConf);
        const auto musicType = PathType::music;
        const auto archiveType = PathType::archive;
        const auto tempRootType = PathType::temp;
        const auto coverArtType = PathType::coverArt;

        auto rootMusicPath = pathConfig[directory_manager::retrievePathType(musicType)];
        auto archiveRootPath = pathConfig[directory_manager::retrievePathType(archiveType)];
        auto tempRootPath = pathConfig[directory_manager::retrievePathType(tempRootType)];
        auto coverRootPath = pathConfig[directory_manager::retrievePathType(coverArtType)];

        vector<string> paths;
        paths.reserve(4);
        paths.emplace_back(std::move(rootMusicPath));
        paths.emplace_back(std::move(archiveRootPath));
        paths.emplace_back(std::move(tempRootPath));
        paths.emplace_back(std::move(coverRootPath));

        return confirmMultiplePaths<string, vector>(paths);
    }

    void Initialization::failedConfirmation()
    {
        cout << "configuration confirmation failed. check your settings\n";
        std::exit(-1);
    }
}
