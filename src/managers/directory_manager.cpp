#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "managers/directory_manager.h"

namespace fs = std::filesystem;

std::string Manager::directory_manager::create_directory_process(Model::Song song, const std::string& root_path)
{
    auto curr_path = fs::path(root_path);

    if (fs::exists(curr_path)) {
        std::cout<<"path exists"<<std::endl;
    } else {
        std::cout<<"creating path"<<std::endl;
        fs::create_directory(curr_path);
    }

    auto art_path = fs::path(curr_path.string() + song.artist);
    if (fs::exists(art_path)) {
        std::cout<<"artist path exists"<<std::endl;
    } else {
        std::cout<<"creating artist path"<<std::endl;
        fs::create_directory(art_path);
    }

    auto alb_path = fs::path(art_path.string() + "/" + song.album);
    if (fs::exists(alb_path)) {
        std::cout<<"album path exists"<<std::endl;
    } else {
        std::cout<<"creating album path"<<std::endl;
        fs::create_directory(alb_path);
    }

    return alb_path.string() + "/";
}

std::string Manager::directory_manager::configPath(std::string_view path)
{
    return fs::canonical(path).parent_path().string();
}
std::string Manager::directory_manager::contentOfPath(std::string_view path)
{
    std::string configPath(path);
    std::fstream a(configPath, std::ios::in);
    std::stringstream s;
    s << a.rdbuf();
    a.close();

    return s.str();
}

nlohmann::json Manager::directory_manager::credentialConfigContent(const std::string& exe_path)
{
    auto path = configPath(exe_path);
    path.append("/authcredentials.json");

    return nlohmann::json::parse(contentOfPath(path));
}
nlohmann::json Manager::directory_manager::databaseConfigContent(const std::string& exe_path)
{
    auto path = configPath(exe_path);
    path.append("/database.json");

    return nlohmann::json::parse(contentOfPath(path));
}
nlohmann::json Manager::directory_manager::pathConfigContent(const std::string& exe_path)
{
    auto path = configPath(exe_path);
    path.append("/paths.json");

    return nlohmann::json::parse(contentOfPath(path));
}

void Manager::directory_manager::delete_cover_art_file(const std::string& cov_path, const std::string& stock_cover_path)
{
    if (cov_path.compare(stock_cover_path) == 0) {
        std::cout << "cover has stock cover art, will not deleted" << std::endl;
    } else {
        std::cout << "deleting song path" << std::endl;
        auto cov = fs::path(cov_path);
        fs::remove(cov);
    }
}
void Manager::directory_manager::delete_directories(Model::Song song, const std::string& root_path)
{
    std::cout<<"checking to for empty directories to delete"<<std::endl;
    const std::string art{root_path + std::string{"/"} + song.artist};
    const std::string alb{art + "/" + song.album};

    auto alb_path = fs::path(alb);
    
    if (!fs::exists(alb_path)) {
        std::cout<<"directory does not exists"<<std::endl;
    } else if (fs::is_empty(alb_path)) {
        fs::remove(alb_path);
    }

    auto art_path = fs::path(art);
    
    if (!fs::exists(art_path)) {
        std::cout<<"directory does not exists"<<std::endl;
        return;
    } else if (fs::is_empty(art_path)) {
        fs::remove(art_path);
    }

    std::cout<<"deleted empty directory or directories"<<std::endl;
}
void Manager::directory_manager::delete_song(const Model::Song song)
{
    std::cout<<"deleting song"<<std::endl;
    auto song_path = fs::path(song.songPath);

    if (!fs::exists(song_path)) {
        std::cout<<"song does not exists"<<std::endl;
        return;
    }

    fs::remove(song_path);
    std::cout<<"deleted song"<<std::endl;
}

