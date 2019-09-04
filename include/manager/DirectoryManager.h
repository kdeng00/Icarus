#ifndef DIRECTORY_MANAGER_H_
#define DIRECTORY_MANAGER_H_

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "model/Models.h"

namespace manager
{
    class DirectoryManager
    {
    public:
        static std::string createDirectoryProcess(model::Song, const std::string&);
        static std::string configPath(std::string_view);
        static std::string configPath(const model::BinaryPath&);
        static std::string contentOfPath(const std::string&);

        static nlohmann::json credentialConfigContent(const model::BinaryPath&);
        static nlohmann::json databaseConfigContent(const model::BinaryPath&);
        static nlohmann::json pathConfigContent(const model::BinaryPath&);

        void deleteCoverArtFile(const std::string&, const std::string&);
        static void deleteDirectories(model::Song, const std::string&);

    private:
        void deleteSong(const model::Song);
    };
}

#endif
