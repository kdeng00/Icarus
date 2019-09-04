#include "manager/SongManager.h"

#include <fstream>
#include <filesystem>
#include <random>

#include <nlohmann/json.hpp>

#include "database/CoverArtRepository.h"
#include "database/SongRepository.h"
#include "manager/AlbumManager.h"
#include "manager/ArtistManager.h"
#include "manager/CoverArtManager.h"
#include "manager/DirectoryManager.h"
#include "manager/GenreManager.h"
#include "manager/YearManager.h"
#include "utility/MetadataRetriever.h"

namespace fs = std::filesystem;

manager::SongManager::SongManager(std::string& x_path)
    : exe_path(x_path)
{ }

manager::SongManager::SongManager(const model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


void manager::SongManager::saveSong(model::Song& song)
{
    saveSongTemp(song);
    utility::MetadataRetriever meta;
    auto data = std::move(song.data);
    song = meta.retrieveMetadata(song.songPath);
    song.data = std::move(data);

    saveMisc(song);

    printSong(song);

    database::SongRepository songRepo(m_bConf);
    songRepo.saveRecord(song);
}

void manager::SongManager::deleteSong(model::Song& song)
{
    // TODO: handle what happens to miscellanes records
    // like Album, Artist, Genre, etc. when a song
    // is deleted
    database::CoverArtRepository covRepo(m_bConf);
    database::SongRepository songRepo(m_bConf);

    song = songRepo.retrieveRecord(song, type::SongFilter::id);
    songRepo.deleteRecord(song);

    model::Cover cov;
    cov.id = song.coverArtId;
    cov = covRepo.retrieveRecord(cov, type::CoverFilter::id);
    covRepo.deleteRecord(cov);

    auto paths = manager::DirectoryManager::pathConfigContent(m_bConf);
    const auto coverArtPath = paths["cover_root_path"].get<std::string>();
    std::string stockCoverArtPath = coverArtPath;
    stockCoverArtPath.append("CoverArt.png");

    if (stockCoverArtPath.compare(cov.imagePath) != 0) {
        fs::remove(cov.imagePath);
        std::cout << "deleting cover art" << std::endl;
    }
    fs::remove(song.songPath);

    manager::DirectoryManager::deleteDirectories(song, paths["root_music_path"].get<std::string>());
    manager::DirectoryManager::deleteDirectories(song, coverArtPath);
}

void manager::SongManager::printSong(const model::Song& song)
{
    std::cout << "\nsong" << std::endl;
    std::cout << "title: " << song.title << std::endl;
    std::cout << "artist: " << song.artist << std::endl;
    std::cout << "album: " << song.album << std::endl;
    std::cout << "genre: " << song.genre << std::endl;
    std::cout << "duration: " << song.duration << std::endl;
    std::cout << "year: " << song.year << std::endl;
    std::cout << "track: " << song.track << std::endl;
    std::cout << "disc: " << song.disc << std::endl;
    std::cout << "song path: " << song.songPath << std::endl;
    std::cout << "cover art id: " << song.coverArtId << std::endl;
    std::cout << "album id: " << song.albumId << std::endl;
    std::cout << "artist id: " << song.artistId << std::endl;
    std::cout << "genre id: " << song.genreId << std::endl;
    std::cout << "year id: " << song.yearId << std::endl;
}

void manager::SongManager::saveSongTemp(model::Song& song)
{
    auto config = manager::DirectoryManager::pathConfigContent(m_bConf);

    auto tmpSongPath = config["temp_root_path"].get<std::string>();
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(1,1000);

    tmpSongPath.append(std::to_string(dist(rng)));
    tmpSongPath.append(".mp3");

    std::fstream s(tmpSongPath, std::fstream::binary | std::fstream::out);
    s.write((char*)&song.data[0], song.data.size());
    s.close();

    song.songPath = tmpSongPath;
}
void manager::SongManager::saveMisc(model::Song& song)
{
    CoverArtManager covMgr(m_bConf);
    auto pathConfigContent = manager::DirectoryManager::pathConfigContent(m_bConf);
    auto coverRootPath = pathConfigContent["cover_root_path"].get<std::string>();
    auto musicRootPath = pathConfigContent["root_music_path"].get<std::string>();

    auto stockCoverPath = manager::DirectoryManager::configPath(m_bConf);
    stockCoverPath.append("/CoverArt.png");

    auto cov = covMgr.saveCover(song, coverRootPath, stockCoverPath);

    auto songPath = manager::DirectoryManager::createDirectoryProcess(song, musicRootPath);
    songPath.append(song.title);
    songPath.append(".mp3");
    if (fs::exists(songPath)) {
        std::cout << "deleting old song with the same metadata" << std::endl;
        fs::remove(songPath);
    }
    std::cout << "copying song to the appropriate directory" << std::endl;
    fs::copy(song.songPath, songPath);
    fs::remove(song.songPath);
    song.songPath = std::move(songPath);

    AlbumManager albMgr(m_bConf);
    auto album = albMgr.saveAlbum(song);
    album = albMgr.retrieveAlbum(album);
    manager::AlbumManager::printAlbum(album);

    ArtistManager artMgr(m_bConf);
    auto artist = artMgr.saveArtist(song);
    artist = artMgr.retrieveArtist(artist);
    manager::ArtistManager::printArtist(artist);

    GenreManager gnrMgr(m_bConf);
    auto genre = gnrMgr.saveGenre(song);
    genre = gnrMgr.retrieveGenre(genre);
    manager::GenreManager::printGenre(genre);

    YearManager yrMgr(m_bConf);
    auto year = yrMgr.saveYear(song);
    year = yrMgr.retrieveYear(year);
    manager::YearManager::printYear(year);

    song.coverArtId = cov.id;
    song.albumId = album.id;
    song.artistId = artist.id;
    song.genreId = genre.id;
    song.yearId = year.id;
}
