#include "manager/SongManager.h"

#include <fstream>
#include <filesystem>
#include <random>

#include <nlohmann/json.hpp>
#include "oatpp/web/protocol/http/outgoing/ChunkedBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "database/CoverArtRepository.h"
#include "database/SongRepository.h"
#include "manager/AlbumManager.h"
#include "manager/ArtistManager.h"
#include "manager/CoverArtManager.h"
#include "manager/DirectoryManager.h"
#include "manager/GenreManager.h"
#include "manager/YearManager.h"
#include "type/PathType.h"
#include "utility/MetadataRetriever.h"

namespace fs = std::filesystem;

manager::SongManager::SongManager(std::string& x_path)
    : exe_path(x_path)
{ }

manager::SongManager::SongManager(const model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


bool manager::SongManager::didSongChange(const model::Song& updatedSong, 
    const model::Song& currSong)
{
    if (!updatedSong.title.empty()) {
        return true;
    }
    if (!updatedSong.artist.empty()) {
        return true;
    }
    if (!updatedSong.album.empty()) {
        return true;
    }
    if (updatedSong.genre.empty()) {
        return true;
    }
    if (updatedSong.year != 0) {
        return true;
    }

    return false;
}

bool manager::SongManager::requiresFilesystemChange(const model::Song& updatedSong, 
    const model::Song& currSong)
{
    if (updatedSong.title.compare(currSong.title) != 0) {
        return true;
    }
    if (updatedSong.artist.compare(currSong.album) != 0) {
        return true;
    }
    if (updatedSong.album.compare(currSong.genre) != 0) {
        return true;
    }

    return false;
}


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

void manager::SongManager::updateSong(model::Song& updatedSong)
{
    database::SongRepository songRepo(m_bConf);
    model::Song currSong(updatedSong.id);

    OATPP_ASSERT_HTTP(songRepo.doesSongExist(currSong, type::SongFilter::id) , oatpp::web::protocol::http::Status::CODE_404, "song does not exist");

    currSong = songRepo.retrieveRecord(currSong, type::SongFilter::id);
    if (!didSongChange(updatedSong, currSong)) {
        std::cout << "no change to the song" << std::endl;
        return;
    }

    assignMiscId(updatedSong, currSong);

    auto changes = changesInSong(updatedSong, currSong);

    utility::MetadataRetriever meta;
    meta.updateMetadata(updatedSong, currSong);
    assignMiscFields(changes, updatedSong, currSong);

    if (requiresFilesystemChange(updatedSong, currSong)) {
        modifySongOnFilesystem(updatedSong, currSong);
    }

    printSong(updatedSong);
    printSong(currSong);

    updateMisc(changes, updatedSong, currSong);
}

model::Song manager::SongManager::songDtoConv(dto::SongDto::ObjectWrapper& songDto)
{
    std::cout << "coverting dto::SongDto to model::Song" << std::endl;
    model::Song song;
    song.id = songDto->id;
    song.title = (songDto->title == nullptr) ? "" : songDto->title->c_str();
    song.album = (songDto->album == nullptr) ? "" : songDto->album->c_str();
    song.artist = (songDto->artist == nullptr) ? "" : songDto->artist->c_str();
    song.genre = (songDto->genre == nullptr) ? "" : songDto->genre->c_str();
    song.year = (songDto->year.getPtr() == nullptr) ? 0 : songDto->year->getValue();
    song.track = (songDto->track.getPtr() == nullptr) ? 0 : songDto->track->getValue();
    song.disc = (songDto->disc.getPtr() == nullptr) ? 0 : songDto->disc->getValue();

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


std::map<type::SongChanged, bool> manager::SongManager::changesInSong(
    const model::Song& updatedSong, const model::Song& currSong)
{
    std::map<type::SongChanged, bool> songChanges;

    std::string_view updatedTitle = updatedSong.title;
    std::string_view updatedArtist = updatedSong.artist;
    std::string_view updatedAlbum = updatedSong.album;
    std::string_view updatedGenre = updatedSong.genre;

    songChanges[type::SongChanged::title] = 
        (currSong.title.compare(updatedTitle) != 0 && 
         updatedTitle.size() > 0) ? true : false;

    songChanges[type::SongChanged::artist] = 
        (currSong.artist.compare(updatedArtist) != 0 && 
         updatedArtist.size() > 0) ? true : false;

    songChanges[type::SongChanged::album] = 
        (currSong.album.compare(updatedAlbum) != 0 && 
         updatedAlbum.size() > 0) ? true : false;

    songChanges[type::SongChanged::genre] = 
        (currSong.genre.compare(updatedGenre) != 0 && 
         updatedGenre.size() > 0) ? true : false;

    songChanges[type::SongChanged::year] =
        (updatedSong.year != 0) ? true : false;

    return songChanges;
}


// used to prevent empty values to appear in the updated song
void manager::SongManager::assignMiscFields(
    std::map<type::SongChanged, bool>& songChanges, model::Song& updatedSong,
    const model::Song& currSong)
{
    std::cout << "assigning miscellanes fields to updated song" << std::endl;
    for (auto scIter = songChanges.begin(); scIter != songChanges.end(); ++scIter) {
        type::SongChanged key = scIter->first;
        bool changed = songChanges.at(key);

        if (!changed) {
            switch (key) {
                case type::SongChanged::title:
                    updatedSong.title = currSong.title;
                    std::cout << "title has not been changed" << std::endl;
                    break;
                case type::SongChanged::artist:
                    updatedSong.artist = currSong.artist;
                    std::cout << "artist has not been changed" << std::endl;
                    break;
                case type::SongChanged::album:
                    updatedSong.album = currSong.album;
                    std::cout << "album has not been changed" << std::endl;
                    break;
                case type::SongChanged::genre:
                    updatedSong.genre = currSong.genre;
                    std::cout << "genre has not been changed" << std::endl;
                    break;
                case::type::SongChanged::year:
                    updatedSong.year = currSong.year;
                    std::cout << "year has not been changed" << std::endl;
                default:
                    break;
            }
        }
    }
}

// used to dump miscellaneous id to the updated song
void manager::SongManager::assignMiscId(model::Song& updatedSong,
    const model::Song& currSong)
{
    std::cout << "assigning miscellaneous Id's to updated song" << std::endl;
    updatedSong.artistId = currSong.artistId;
    updatedSong.albumId = currSong.albumId;
    updatedSong.genreId = currSong.genreId;
    updatedSong.yearId = currSong.yearId;
    updatedSong.coverArtId = currSong.coverArtId;
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

void manager::SongManager::updateMisc(
    const std::map<type::SongChanged, bool>& songChanges,
    model::Song& updatedSong, const model::Song& currSong)
{
    auto titleChange = songChanges.at(type::SongChanged::title);
    auto artistChange = songChanges.at(type::SongChanged::artist);
    auto albumChange = songChanges.at(type::SongChanged::album);
    auto genreChange = songChanges.at(type::SongChanged::genre);
    auto yearChange = songChanges.at(type::SongChanged::year);

    if (artistChange) {
        manager::ArtistManager artMgr(m_bConf);
        artMgr.updateArtist(updatedSong, currSong);
    }
    if (albumChange) {
        manager::AlbumManager albMgr(m_bConf);
        albMgr.updateAlbum(updatedSong, currSong);
    }
    if (genreChange) {
        manager::GenreManager gnrMgr(m_bConf);
        gnrMgr.updateGenre(updatedSong, currSong);
    }
    if (yearChange) {
        manager::YearManager yrMgr(m_bConf);
        yrMgr.updateYear(updatedSong, currSong);
    }

    // determins to update the cover art record
    if (titleChange || artistChange || albumChange) {
        manager::CoverArtManager covMgr(m_bConf);
        covMgr.updateCoverRecord(updatedSong);
    }

    database::SongRepository songRepo(m_bConf);
    songRepo.updateRecord(updatedSong);
}

void manager::SongManager::modifySongOnFilesystem(model::Song& updatedSong, 
    const model::Song& currSong)
{
    std::cout << "preparing to modify song" << std::endl;
    printSong(updatedSong);
    auto songPath = manager::DirectoryManager::createDirectoryProcess(
        updatedSong, m_bConf, type::PathType::music);
    songPath.append(updatedSong.title);
    songPath.append(".mp3");
    updatedSong.songPath = std::move(songPath);

    std::cout << "new path " << updatedSong.songPath << std::endl;

    fs::copy(currSong.songPath, updatedSong.songPath);
    fs::remove(currSong.songPath);

    manager::CoverArtManager covMgr(m_bConf);
    covMgr.updateCover(updatedSong);
}
