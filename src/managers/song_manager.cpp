#include "managers/song_manager.h"

#include <fstream>
#include <filesystem>
#include <random>

#include <nlohmann/json.hpp>

#include "database/songRepository.h"
#include "managers/coverArtManager.h"
#include "managers/directory_manager.h"
#include "utilities/metadata_retriever.h"

namespace fs = std::filesystem;

song_manager::song_manager(std::string& x_path)
    : exe_path(x_path)
{ }


void song_manager::saveSong(Song& song)
{
    saveSongTemp(song);
    metadata_retriever meta;
    auto data = std::move(song.data);
    song = meta.retrieve_metadata(song.songPath);
    song.data = std::move(data);
    printSong(song);

    coverArtManager covMgr(exe_path);
    auto coverRootPath = directory_manager::pathConfigContent(exe_path)["cover_root_path"].get<std::string>();
    //const nlohmann::json databaseConfig = directory_manager::databaseConfigContent(exe_path);

    auto stockCoverPath = directory_manager::configPath(exe_path);
    stockCoverPath.append("/CoverArt.png");

    auto cov = covMgr.saveCover(song, coverRootPath, stockCoverPath);
    song.coverArtId = cov.id;

    printSong(song);

    songRepository songRepo(exe_path);
    songRepo.saveRecord(song);
}

void song_manager::printSong(const Song& song)
{
    std::cout << "song" << std::endl;
    std::cout << "title: " << song.title << std::endl;
    std::cout << "artist: " << song.artist << std::endl;
    std::cout << "album: " << song.album << std::endl;
    std::cout << "genre: " << song.genre << std::endl;
    std::cout << "duration: " << song.duration << std::endl;
    std::cout << "year: " << song.year << std::endl;
    std::cout << "song path: " << song.songPath << std::endl;
    if (song.coverArtId != 0) {
        std::cout << "cover art id: " << song.coverArtId << std::endl;
    }
}

void song_manager::saveSongTemp(Song& song)
{
    auto config = directory_manager::pathConfigContent(exe_path);

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
