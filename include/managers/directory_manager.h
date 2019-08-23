#ifndef DIRECTORY_MANAGER_H_
#define DIRECTORY_MANAGER_H_

#include <string>
#include <string_view>

#include <nlohmann/json.hpp>

#include "models/models.h"

class directory_manager
{
public:

    std::string create_directory_process(Song, const std::string&);
    static std::string configPath(std::string_view);
    static std::string contentOfPath(std::string_view);

    static nlohmann::json credentialConfigContent(const std::string&);
    static nlohmann::json databaseConfigContent(const std::string&);
    static nlohmann::json pathConfigContent(const std::string&);
    // Return Cover instead
    //std::string read_cover_art(const std::string&);


    //void copy_stock_to_root(const std::string&, const std::string&);
    //void copy_song_to_path(const std::string&, const std::string&);
    void delete_cover_art_file(const std::string&, const std::string&);
    void delete_directories(Song, const std::string&);

private:
    void delete_song(const Song);
};

#endif
