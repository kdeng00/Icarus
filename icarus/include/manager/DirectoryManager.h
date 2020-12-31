#ifndef DIRECTORY_MANAGER_H_
#define DIRECTORY_MANAGER_H_

#include <string>
#include <string_view>
#include <filesystem>

#include "icarus_lib/icarus.h"
#include <nlohmann/json.hpp>

#include "type/PathType.h"

namespace manager {
    class DirectoryManager {
    public:
        DirectoryManager() = default;

        static std::string createDirectoryProcess(const icarus_lib::song&, const std::string&);
        static std::string createDirectoryProcess(const icarus_lib::song&, 
                const icarus_lib::binary_path &, type::PathType);
        static std::string configPath(std::string_view);
        static std::string configPath(const icarus_lib::binary_path &);
        static std::string contentOfPath(const std::string&);
        static std::string retrievePathType(type::PathType);

        static nlohmann::json credentialConfigContent(const icarus_lib::binary_path &);

        static nlohmann::json databaseConfigContent(const icarus_lib::binary_path &);

        template<typename json = nlohmann::json,
                 typename config_t = icarus_lib::binary_path>
        static json keyConfigContent(const config_t &config)
        {
            auto path = configPath(config);
            path.append("/");
            path.append(icarusKeyConfigName());

            return json::parse(contentOfPath(path));
        }

        static nlohmann::json pathConfigContent(const icarus_lib::binary_path &);

        static constexpr auto icarusKeyConfigName() noexcept
        {
            return "icarus_keys.json";
        }

        static void deleteDirectories(icarus_lib::song, const std::string&);

        void deleteCoverArtFile(const std::string&, const std::string&);

    private:
        std::filesystem::path relativeDiscSongPathFilesystem(const std::filesystem::path&, 
                const icarus_lib::song&);
        std::string relativeDiscSongPath(const std::filesystem::path&, const icarus_lib::song&);

        void deleteSong(const icarus_lib::song);
    };
}

#endif
