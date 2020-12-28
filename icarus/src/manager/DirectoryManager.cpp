#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "manager/DirectoryManager.h"

namespace fs = std::filesystem;

namespace manager {
    std::string DirectoryManager::createDirectoryProcess(const model::Song& song, 
            const std::string& rootPath) {
        auto currPath = fs::path(rootPath);

        if (fs::exists(currPath)) {
            std::cout << "path exists\n";
        } else {
            std::cout << "creating path\n";
            fs::create_directory(currPath);
        }

        auto artPath = fs::path(currPath.string() + song.artist);
        if (fs::exists(artPath)) {
            std::cout << "artist path exists\n";
        } else {
            std::cout << "creating artist path\n";
            fs::create_directory(artPath);
        }

        auto albPath = fs::path(artPath.string() + "/" + song.albumArtist);
        if (fs::exists(albPath)) {
            std::cout << "album path exists\n";
        } else {
            std::cout << "creating album path\n";
            fs::create_directory(albPath);
        }

        auto discPath = DirectoryManager().relativeDiscSongPathFilesystem(albPath, song);

        if (fs::exists(discPath)) {
            std::cout << "disc path exists\n";
        } else {
            std::cout << "creating disc path\n";
            fs::create_directory(discPath);
        }

        return discPath.string() + "/";
    }

    std::string DirectoryManager::createDirectoryProcess(const model::Song& song, 
            const icarus_lib::binary_path & bConf, type::PathType pType) {
        auto path = pathConfigContent(bConf)[retrievePathType(pType)];
        auto rootPath = path.get<std::string>();
        auto currPath = fs::path(rootPath);

        if (fs::exists(currPath)) {
            std::cout << "path exists\n";
        } else {
            std::cout << "creating root music path\n";
            fs::create_directory(currPath);
        }

        auto artPath = fs::path(currPath.string() + song.albumArtist);
        if (fs::exists(artPath)) {
            std::cout << "artist path exists\n";
        } else {
            std::cout << "creating artist path\n";
            fs::create_directory(artPath);
        }

        auto albPath = fs::path(artPath.string() + "/" + song.album);
        if (fs::exists(albPath)) { 
            std::cout << "album path exists\n";
        } else {
            std::cout << "creating album path\n";
            fs::create_directory(albPath);
        }

        auto discPath = DirectoryManager().relativeDiscSongPathFilesystem(albPath, song);
        if (fs::exists(discPath)) {
            std::cout << "disc path exists\n";
        } else {
            std::cout << "creating disc path\n";
            fs::create_directory(discPath);
        }

        return discPath.string() + "/";
    }

    std::string DirectoryManager::configPath(std::string_view path) {
        return fs::canonical(path).parent_path().string();
    }

    std::string DirectoryManager::configPath(const icarus_lib::binary_path & bConf) {
        return fs::canonical(bConf.path).parent_path().string();
    }

    std::string DirectoryManager::contentOfPath(const std::string& path) {
        std::fstream a(path, std::ios::in);
        std::stringstream s;
        s << a.rdbuf();
        a.close();

        return s.str();
    }

    std::string DirectoryManager::retrievePathType(type::PathType pType) {
        std::string path;
        switch (pType) {
            case type::PathType::music:
                path = "root_music_path";
                break;
            case type::PathType::archive:
                path = "archive_root_path";
                break;
            case type::PathType::temp:
                path = "temp_root_path";
                break;
            case type::PathType::coverArt:
                path = "cover_root_path";
                break;
            default:
                break;
        }

        return path;
    }


    nlohmann::json DirectoryManager::credentialConfigContent(const icarus_lib::binary_path & bConf) {
        auto path = configPath(bConf);
        path.append("/authcredentials.json");

        return nlohmann::json::parse(contentOfPath(path));
    }

    nlohmann::json DirectoryManager::databaseConfigContent(const icarus_lib::binary_path & bConf) {
        auto path = configPath(bConf);
        path.append("/database.json");

        return nlohmann::json::parse(contentOfPath(path));
    }

    nlohmann::json DirectoryManager::pathConfigContent(const icarus_lib::binary_path & bConf) {
        auto path = configPath(bConf);
        path.append("/paths.json");

        return nlohmann::json::parse(contentOfPath(path));
    }


    void DirectoryManager::deleteDirectories(model::Song song, const std::string& rootPath) {
        std::cout << "checking for empty directories to delete\n";
        const std::string art(rootPath + std::string("/") + song.albumArtist);
        const std::string alb(art + "/" + song.album);
        const std::string disc(alb + "/" + std::to_string(song.disc) + "/");

        auto discPath = fs::path(disc);

        if (fs::exists(discPath)) {
            std::cout << "disc directory does not exist\n";
        } else if (fs::is_empty(discPath)) {
            fs::remove(discPath);
        }

        auto albPath = fs::path(alb);
    
        if (!fs::exists(albPath)) {
            std::cout << "directory does not exists\n";
        } else if (fs::is_empty(albPath)) {
            fs::remove(albPath);
        }

        auto artPath = fs::path(art);
    
        if (!fs::exists(artPath)) {
            std::cout << "directory does not exists\n";
            return;
        } else if (fs::is_empty(artPath)) {
            fs::remove(artPath);
        }

        std::cout << "deleted empty directory or directories\n";
    }

    void DirectoryManager::deleteCoverArtFile(const std::string& covPath, 
            const std::string& stockCoverPath) {
        if (covPath.compare(stockCoverPath) == 0) {
            std::cout << "cover has stock cover art, will not deleted\n";
        } else {
            std::cout << "deleting song path\n";
            auto cov = fs::path(covPath);
            fs::remove(cov);
        }
    }


    fs::path DirectoryManager::relativeDiscSongPathFilesystem(const fs::path& albPath, 
            const model::Song& song) {
        std::string albPathStr(albPath.string() + "/disc");
        if (song.disc >= 10) {
            albPathStr.append(std::to_string(song.disc));
        } else {
            albPathStr.append("0");
            albPathStr.append(std::to_string(song.disc));
        }

        albPathStr.append("/");
        auto relPath = fs::path(albPathStr.c_str());
        
        return relPath;
    }


    std::string DirectoryManager::relativeDiscSongPath(const fs::path& albPath, 
            const model::Song& song) {
        std::string albPathStr(albPath.string() + "/disc");
        if (song.disc >= 10) {
            albPathStr.append(std::to_string(song.disc));
        } else {
            albPathStr.append("0");
            albPathStr.append(std::to_string(song.disc));
        }

        albPathStr.append("/");
        return albPathStr;
    }


    void DirectoryManager::deleteSong(const model::Song song) {
        std::cout << "deleting song\n";
        auto songPath = fs::path(song.songPath);

        if (!fs::exists(songPath)) {
            std::cout << "song does not exists\n";
            return;
        }

        fs::remove(songPath);
        std::cout << "deleted song" << "\n";
    }
}
