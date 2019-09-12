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
    database::SongRepository songRepo(m_bConf);

    if (!songRepo.doesSongExist(song, type::SongFilter::id)) {
        std::cout << "song does not exist" << std::endl;
        return;
    }

    song = songRepo.retrieveRecord(song, type::SongFilter::id);

    auto paths = manager::DirectoryManager::pathConfigContent(m_bConf);

    auto deleted = songRepo.deleteRecord(song);

    if (!deleted) {
        std::cout << "song not deleted from databases" << std::endl;
        return;
    }
    deleteMisc(song);

    fs::remove(song.songPath);
    manager::DirectoryManager::deleteDirectories(song, paths["root_music_path"].get<std::string>());
}

void manager::SongManager::updateSong(model::Song& song)
{
    // TODO: update song metadata
}

model::Song manager::SongManager::songDtoConv(dto::SongDto::ObjectWrapper& songDto)
{
    // TODO: figure out how to determine if a non-character sequence 
    // field is empty. Will be necessary for updating the
    // song based on what the HTTP request body is
    std::cout << "coverting dto::SongDto to model::Song" << std::endl;
    model::Song song;
    song.id = songDto->id;
    song.album = (songDto->album == nullptr) ? "" : songDto->album->c_str();
    song.artist = (songDto->artist == nullptr) ? "" : songDto->artist->c_str();
    song.genre = (songDto->genre == nullptr) ? "" : songDto->genre->c_str();
    auto s = songDto->year.empty();
    std::cout << s << std::endl;
    auto sy = &songDto->year;
    if (sy->empty()) {
    //}
    //if (true) {
        std::cout << "year is null" << std::endl;
    } else {
        std::cout << "year is not null" << std::endl;
    }
    song.title = (songDto->title == nullptr) ? "" : songDto->title->c_str();
    //song.track = songDto->track;
    //song.duration = songDto->duration;
    //song.disc = songDto->disc;

    return song;
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
    std::cout << "out" << std::endl;
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

    std::cout << "done with miscellaneous database records" << std::endl;
}

void manager::SongManager::deleteMisc(const model::Song& song)
{
    manager::CoverArtManager covMgr(m_bConf);
    covMgr.deleteCover(song);

    manager::AlbumManager albMgr(m_bConf);
    albMgr.deleteAlbum(song);

    manager::ArtistManager artMgr(m_bConf);
    artMgr.deleteArtist(song);

    manager::GenreManager gnrMgr(m_bConf);
    gnrMgr.deleteGenre(song);

    manager::YearManager yrMgr(m_bConf);
    yrMgr.deleteYear(song);
}
