#ifndef DIRECTORY_MANAGER_H_
#define DIRECTORY_MANAGER_H_

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "models/models.h"

namespace Manager
{
    class directory_manager
    {
    public:
        static std::string create_directory_process(Model::Song, const std::string&);
        static std::string configPath(std::string_view);
        static std::string configPath(const Model::BinaryPath&);
        static std::string contentOfPath(const std::string&);

        //static nlohmann::json credentialConfigContent(const std::string&);
        static nlohmann::json credentialConfigContent(const Model::BinaryPath&);
        //static nlohmann::json databaseConfigContent(const std::string&);
        static nlohmann::json databaseConfigContent(const Model::BinaryPath&);
        //static nlohmann::json pathConfigContent(const std::string&);
        static nlohmann::json pathConfigContent(const Model::BinaryPath&);

        void delete_cover_art_file(const std::string&, const std::string&);
        static void delete_directories(Model::Song, const std::string&);

    private:
        void delete_song(const Model::Song);
    };
}

#endif
