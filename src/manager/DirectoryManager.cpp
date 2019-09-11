#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "manager/DirectoryManager.h"

namespace fs = std::filesystem;

std::string manager::DirectoryManager::createDirectoryProcess(model::Song song, const std::string& rootPath)
{
    auto currPath = fs::path(rootPath);

    if (fs::exists(currPath)) {
        std::cout << "path exists" << std::endl;
    } else {
        std::cout << "creating path" << std::endl;
        fs::create_directory(currPath);
    }

    auto artPath = fs::path(currPath.string() + song.artist);
    if (fs::exists(artPath)) {
        std::cout << "artist path exists" << std::endl;
    } else {
        std::cout << "creating artist path" << std::endl;
        fs::create_directory(artPath);
    }

    auto albPath = fs::path(artPath.string() + "/" + song.album);
    if (fs::exists(albPath)) {
        std::cout << "album path exists" << std::endl;
    } else {
        std::cout << "creating album path" << std::endl;
        fs::create_directory(albPath);
    }

    return albPath.string() + "/";
}

std::string manager::DirectoryManager::configPath(std::string_view path)
{
    return fs::canonical(path).parent_path().string();
}
std::string manager::DirectoryManager::configPath(const model::BinaryPath& bConf)
{
    return fs::canonical(bConf.path).parent_path().string();
}
std::string manager::DirectoryManager::contentOfPath(const std::string& path)
{
    std::fstream a(path, std::ios::in);
    std::stringstream s;
    s << a.rdbuf();
    a.close();

    return s.str();
}

nlohmann::json manager::DirectoryManager::credentialConfigContent(const model::BinaryPath& bConf)
{
    auto path = configPath(bConf);
    path.append("/authcredentials.json");

    return nlohmann::json::parse(contentOfPath(path));
}
nlohmann::json manager::DirectoryManager::databaseConfigContent(const model::BinaryPath& bConf)
{
    auto path = configPath(bConf);
    path.append("/database.json");

    return nlohmann::json::parse(contentOfPath(path));
}
nlohmann::json manager::DirectoryManager::pathConfigContent(const model::BinaryPath& bConf)
{
    auto path = configPath(bConf);
    path.append("/paths.json");

    return nlohmann::json::parse(contentOfPath(path));
}

void manager::DirectoryManager::deleteCoverArtFile(const std::string& covPath, const std::string& stockCoverPath)
{
    if (covPath.compare(stockCoverPath) == 0) {
        std::cout << "cover has stock cover art, will not deleted" << std::endl;
    } else {
        std::cout << "deleting song path" << std::endl;
        auto cov = fs::path(covPath);
        fs::remove(cov);
    }
}
void manager::DirectoryManager::deleteDirectories(model::Song song, const std::string& rootPath)
{
    std::cout << "checking for empty directories to delete" << std::endl;
    const std::string art(rootPath + std::string("/") + song.artist);
    const std::string alb(art + "/" + song.album);

    auto albPath = fs::path(alb);
    
    if (!fs::exists(albPath)) {
        std::cout << "directory does not exists" << std::endl;
    } else if (fs::is_empty(albPath)) {
        fs::remove(albPath);
    }

    auto artPath = fs::path(art);
    
    if (!fs::exists(artPath)) {
        std::cout << "directory does not exists" << std::endl;
        return;
    } else if (fs::is_empty(artPath)) {
        fs::remove(artPath);
    }

    std::cout << "deleted empty directory or directories" << std::endl;
}
void manager::DirectoryManager::deleteSong(const model::Song song)
{
    std::cout << "deleting song" << std::endl;
    auto songPath = fs::path(song.songPath);

    if (!fs::exists(songPath)) {
        std::cout << "song does not exists" << std::endl;
        return;
    }

    fs::remove(songPath);
    std::cout << "deleted song" << std::endl;
}

