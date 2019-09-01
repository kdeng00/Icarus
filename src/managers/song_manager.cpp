#include "managers/song_manager.h"

#include <fstream>
#include <filesystem>
#include <random>

#include <nlohmann/json.hpp>

#include "database/coverArtRepository.h"
#include "database/songRepository.h"
#include "managers/coverArtManager.h"
#include "managers/directory_manager.h"
#include "utilities/metadata_retriever.h"

namespace fs = std::filesystem;

Manager::song_manager::song_manager(std::string& x_path)
    : exe_path(x_path)
{ }


void Manager::song_manager::saveSong(Model::Song& song)
{
    saveSongTemp(song);
    metadata_retriever meta;
    auto data = std::move(song.data);
    song = meta.retrieve_metadata(song.songPath);
    song.data = std::move(data);

    coverArtManager covMgr(exe_path);
    auto pathConfigContent = Manager::directory_manager::pathConfigContent(exe_path);
    auto coverRootPath = pathConfigContent["cover_root_path"].get<std::string>();
    auto musicRootPath = pathConfigContent["root_music_path"].get<std::string>();

    auto stockCoverPath = Manager::directory_manager::configPath(exe_path);
    stockCoverPath.append("/CoverArt.png");

    auto cov = covMgr.saveCover(song, coverRootPath, stockCoverPath);
    song.coverArtId = cov.id;

    auto songPath = Manager::directory_manager::create_directory_process(song, musicRootPath);
    songPath.append(song.title);
    songPath.append(".mp3");
    std::cout << "\n\ntemp path: " << song.songPath << std::endl;
    std::cout << "new path: " << songPath << std::endl;
    fs::copy(song.songPath, songPath);
    fs::remove(song.songPath);
    song.songPath = std::move(songPath);

    printSong(song);

    Database::songRepository songRepo(exe_path);
    songRepo.saveRecord(song);
}

void Manager::song_manager::deleteSong(Model::Song& song)
{
    Database::coverArtRepository covRepo(exe_path);
    Database::songRepository songRepo(exe_path);

    song = songRepo.retrieveRecord(song, songFilter::id);
    songRepo.deleteRecord(song);

    Model::Cover cov;
    cov.id = song.coverArtId;
    cov = covRepo.retrieveRecord(cov, coverFilter::id);
    covRepo.deleteRecord(cov);

    auto paths = Manager::directory_manager::pathConfigContent(exe_path);
    const auto coverArtPath = paths["cover_root_path"].get<std::string>();
    std::string stockCoverArtPath = coverArtPath;
    stockCoverArtPath.append("CoverArt.png");

    if (stockCoverArtPath.compare(cov.imagePath) != 0) {
        fs::remove(cov.imagePath);
        std::cout << "deleting cover art" << std::endl;
    }
    fs::remove(song.songPath);

    Manager::directory_manager::delete_directories(song, paths["root_music_path"].get<std::string>());
    Manager::directory_manager::delete_directories(song, coverArtPath);
}

void Manager::song_manager::printSong(const Model::Song& song)
{
    std::cout << "\n\nsong" << std::endl;
    std::cout << "title: " << song.title << std::endl;
    std::cout << "artist: " << song.artist << std::endl;
    std::cout << "album: " << song.album << std::endl;
    std::cout << "genre: " << song.genre << std::endl;
    std::cout << "duration: " << song.duration << std::endl;
    std::cout << "year: " << song.year << std::endl;
    std::cout << "track: " << song.track << std::endl;
    std::cout << "disc: " << song.disc << std::endl;
    std::cout << "song path: " << song.songPath << std::endl;
    if (song.coverArtId != 0) {
        std::cout << "cover art id: " << song.coverArtId << std::endl;
    }
}

void Manager::song_manager::saveSongTemp(Model::Song& song)
{
    auto config = Manager::directory_manager::pathConfigContent(exe_path);

    auto tmp_song = config["temp_root_path"].get<std::string>();
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1,1000);

    tmp_song.append(std::to_string(dist(rng)));
    tmp_song.append(".mp3");

    std::fstream s(tmp_song, std::fstream::binary | std::fstream::out);
    s.write((char*)&song.data[0], song.data.size());
    s.close();

    song.songPath = tmp_song;
}
