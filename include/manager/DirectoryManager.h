#ifndef DIRECTORY_MANAGER_H_
#define DIRECTORY_MANAGER_H_

#include <string>
#include <string_view>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "model/Models.h"
#include "type/PathType.h"

namespace manager {
    class DirectoryManager {
    public:
        DirectoryManager() = default;

        static std::string createDirectoryProcess(const model::Song&, const std::string&);
        static std::string createDirectoryProcess(const model::Song&, 
                const model::BinaryPath&, type::PathType);
        static std::string configPath(std::string_view);
        static std::string configPath(const model::BinaryPath&);
        static std::string contentOfPath(const std::string&);
        static std::string retrievePathType(type::PathType);

        static nlohmann::json credentialConfigContent(const model::BinaryPath&);
        static nlohmann::json databaseConfigContent(const model::BinaryPath&);
        static nlohmann::json pathConfigContent(const model::BinaryPath&);

        static void deleteDirectories(model::Song, const std::string&);

        void deleteCoverArtFile(const std::string&, const std::string&);

    private:
        std::filesystem::path relativeDiscSongPathFilesystem(const std::filesystem::path&, 
                const model::Song&);
        std::string relativeDiscSongPath(const std::filesystem::path&, const model::Song&);

        void deleteSong(const model::Song);
    };
}

#endif
