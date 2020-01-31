#include "manager/SongManager.h"

#include <fstream>
#include <filesystem>
#include <random>

#include <nlohmann/json.hpp>
//#include "oatpp/web/protocol/http/outgoing/ChunkedBody.hpp"
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

namespace manager {
    SongManager::SongManager(const model::BinaryPath& bConf) : m_bConf(bConf) { }


    std::pair<bool, type::SongUpload> SongManager::saveSong(model::Song& song) {
		saveSongTemp(song);
		utility::MetadataRetriever meta;
		auto data = std::move(song.data);
		song = meta.retrieveMetadata(song);
		song.data = std::move(data);

		database::SongRepository songRepo(m_bConf);
		if (songRepo.doesSongExist(song, type::SongFilter::titleAndArtist)) {
		    std::cout << "\ntitle: " << song.title << "\nartist: " << song.artist << "\n";
		    std::cout << "does not exist\n";
		    return std::make_pair(false, type::SongUpload::AlreadyExist);
		}

		saveMisc(song);

		printSong(song);

		songRepo.saveRecord(song);
		song = songRepo.retrieveRecord(song, type::SongFilter::titleAndArtist);

		return std::make_pair(true, type::SongUpload::Successful);
    }


    bool SongManager::didSongChange(const model::Song& updatedSong, 
		    const model::Song& currSong) {
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

    bool SongManager::requiresFilesystemChange(const model::Song& updatedSong, 
		    const model::Song& currSong) {
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

    bool SongManager::deleteSong(model::Song& song) {
		database::SongRepository songRepo(m_bConf);

		if (!songRepo.doesSongExist(song, type::SongFilter::id)) {
		    std::cout << "song does not exist\n";
		    return false;
		}

		song = songRepo.retrieveRecord(song, type::SongFilter::id);

		auto paths = DirectoryManager::pathConfigContent(m_bConf);

		auto deleted = songRepo.deleteRecord(song);

		if (!deleted) {
		    std::cout << "song not deleted from databases\n";
		    return deleted;
		}
		deleteMisc(song);

		fs::remove(song.songPath);
		DirectoryManager::deleteDirectories(song, paths["root_music_path"].get<std::string>());
		return deleted;
    }

    bool SongManager::updateSong(model::Song& updatedSong) {
		database::SongRepository songRepo(m_bConf);
		model::Song currSong(updatedSong.id);

		currSong = songRepo.retrieveRecord(currSong, type::SongFilter::id);
		if (!didSongChange(updatedSong, currSong)) {
		    std::cout << "no change to the song\n";
		    return false;
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

		return true;
    }


    void SongManager::printSong(const model::Song& song) {
		std::cout << "\nsong" << "\n";
		std::cout << "title: " << song.title << "\n";
		std::cout << "artist: " << song.artist << "\n";
		std::cout << "album artist: " << song.albumArtist << "\n";
		std::cout << "album: " << song.album << "\n";
		std::cout << "genre: " << song.genre << "\n";
		std::cout << "duration: " << song.duration << "\n";
		std::cout << "year: " << song.year << "\n";
		std::cout << "track: " << song.track << "\n";
		std::cout << "disc: " << song.disc << "\n";
		std::cout << "song path: " << song.songPath << "\n";
		std::cout << "cover art id: " << song.coverArtId << "\n";
		std::cout << "album id: " << song.albumId << "\n";
		std::cout << "artist id: " << song.artistId << "\n";
		std::cout << "genre id: " << song.genreId << "\n";
		std::cout << "year id: " << song.yearId << "\n";
    }


    std::map<type::SongChanged, bool> SongManager::changesInSong(
		    const model::Song& updatedSong, const model::Song& currSong) {
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


    std::string SongManager::createSongPath(const model::Song& song) {
		auto songPath = DirectoryManager::createDirectoryProcess(
				song, m_bConf, type::PathType::music);

		if (song.track != 0) {
		    songPath.append("track");
		    auto trackNum = (song.track > 9) ?
				    std::to_string(song.track) : "0" + std::to_string(song.track);
		    songPath.append(trackNum);
		} else {
		    songPath.append(song.title);
		}
		songPath.append(".mp3");

		return songPath;
    }


    // used to prevent empty values to appear in the updated song
    void SongManager::assignMiscFields(
		    std::map<type::SongChanged, bool>& songChanges, model::Song& updatedSong,
		    const model::Song& currSong) {
		std::cout << "assigning miscellanes fields to updated song\n";
		updatedSong.track = currSong.track;
		for (auto scIter = songChanges.begin(); scIter != songChanges.end(); ++scIter) {
		    type::SongChanged key = scIter->first;
		    bool changed = songChanges.at(key);

		    if (!changed) {
                switch (key) {
		            case type::SongChanged::title:
		                updatedSong.title = currSong.title;
		                std::cout << "title has not been changed\n";
		                break;
                    case type::SongChanged::artist:
                        updatedSong.artist = currSong.artist;
                        std::cout << "artist has not been changed\n";
                        break;
                    case type::SongChanged::album:
                        updatedSong.album = currSong.album;
                        std::cout << "album has not been changed\n";
                        break;
                    case type::SongChanged::genre:
                        updatedSong.genre = currSong.genre;
                        std::cout << "genre has not been changed\n";
                        break;
                    case::type::SongChanged::year:
                        updatedSong.year = currSong.year;
                        std::cout << "year has not been changed\n";
                    default:
                        break;
                }
            }
        }
    }

    // used to dump miscellaneous id to the updated song
    void SongManager::assignMiscId(model::Song& updatedSong,
            const model::Song& currSong) {
        std::cout << "assigning miscellaneous Id's to updated song\n";
        updatedSong.artistId = currSong.artistId;
        updatedSong.albumId = currSong.albumId;
        updatedSong.genreId = currSong.genreId;
        updatedSong.yearId = currSong.yearId;
        updatedSong.coverArtId = currSong.coverArtId;
    }

    // saves song to a temporary path
    void SongManager::saveSongTemp(model::Song& song) {
        auto config = DirectoryManager::pathConfigContent(m_bConf);

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

    void SongManager::saveMisc(model::Song& song) {
        CoverArtManager covMgr(m_bConf);
        auto pathConfigContent = DirectoryManager::pathConfigContent(m_bConf);
        auto musicRootPath = pathConfigContent["root_music_path"].get<std::string>();

        auto cov = covMgr.saveCover(song);
        const auto songPath = createSongPath(song);

        if (fs::exists(songPath)) {
            std::cout << "deleting old song with the same metadata\n";
            fs::remove(songPath);
        }
        std::cout << "copying song to the appropriate directory\n";
        std::cout << song.songPath << std::endl;
        std::cout << songPath << std::endl;
        fs::copy(song.songPath, songPath);
        fs::remove(song.songPath);
        song.songPath = std::move(songPath);
        std::cout << "copied song to the appropriate directory\n";

        AlbumManager albMgr(m_bConf);
        auto album = albMgr.saveAlbum(song);
        album = albMgr.retrieveAlbum(album);
        AlbumManager::printAlbum(album);

        ArtistManager artMgr(m_bConf);
        auto artist = artMgr.saveArtist(song);
        artist = artMgr.retrieveArtist(artist);
        ArtistManager::printArtist(artist);

        GenreManager gnrMgr(m_bConf);
        auto genre = gnrMgr.saveGenre(song);
        genre = gnrMgr.retrieveGenre(genre);
        GenreManager::printGenre(genre);

        YearManager yrMgr(m_bConf);
        auto year = yrMgr.saveYear(song);
        year = yrMgr.retrieveYear(year);
        YearManager::printYear(year);

        song.coverArtId = cov.id;
        song.albumId = album.id;
        song.artistId = artist.id;
        song.genreId = genre.id;
        song.yearId = year.id;

        std::cout << "done with miscellaneous database records\n";
    }

    void SongManager::deleteMisc(const model::Song& song) {
        CoverArtManager covMgr(m_bConf);
        covMgr.deleteCover(song);

        AlbumManager albMgr(m_bConf);
        albMgr.deleteAlbum(song);

        ArtistManager artMgr(m_bConf);
        artMgr.deleteArtist(song);

        GenreManager gnrMgr(m_bConf);
        gnrMgr.deleteGenre(song);

        YearManager yrMgr(m_bConf);
        yrMgr.deleteYear(song);
    }

    // deletes miscellanes records
    void SongManager::deleteMiscExceptCoverArt(const model::Song& song) {
        AlbumManager albMgr(m_bConf);
        albMgr.deleteAlbum(song);

        ArtistManager artMgr(m_bConf);
        artMgr.deleteArtist(song);

        GenreManager gnrMgr(m_bConf);
        gnrMgr.deleteGenre(song);

        YearManager yrMgr(m_bConf);
        yrMgr.deleteYear(song);
    }

    void SongManager::updateMisc(
            const std::map<type::SongChanged, bool>& songChanges,
            model::Song& updatedSong, const model::Song& currSong) {
        auto titleChange = songChanges.at(type::SongChanged::title);
        auto artistChange = songChanges.at(type::SongChanged::artist);
        auto albumChange = songChanges.at(type::SongChanged::album);
        auto genreChange = songChanges.at(type::SongChanged::genre);
        auto yearChange = songChanges.at(type::SongChanged::year);

        if (artistChange) {
            ArtistManager artMgr(m_bConf);
            artMgr.updateArtist(updatedSong, currSong);
        }
        if (albumChange) {
            AlbumManager albMgr(m_bConf);
            albMgr.updateAlbum(updatedSong, currSong);
        }
        if (genreChange) {
            GenreManager gnrMgr(m_bConf);
            gnrMgr.updateGenre(updatedSong, currSong);
        }
        if (yearChange) {
            YearManager yrMgr(m_bConf);
            yrMgr.updateYear(updatedSong, currSong);
        }

        // determins to update the cover art record
        if (titleChange || artistChange || albumChange) {
            CoverArtManager covMgr(m_bConf);
            covMgr.updateCoverRecord(updatedSong);
        }

        database::SongRepository songRepo(m_bConf);
        songRepo.updateRecord(updatedSong);

        deleteMiscExceptCoverArt(currSong);
    }

    void SongManager::modifySongOnFilesystem(model::Song& updatedSong, 
            const model::Song& currSong) {
        std::cout << "preparing to modify song\n";
        auto songPath = createSongPath(updatedSong);
        updatedSong.songPath = std::move(songPath);

        std::cout << "new path " << updatedSong.songPath << "\n";

        fs::copy(currSong.songPath, updatedSong.songPath);
        fs::remove(currSong.songPath);

        auto paths = DirectoryManager::pathConfigContent(m_bConf);
        const auto musicRootPath = 
            paths[DirectoryManager::retrievePathType(
                type::PathType::music)].get<std::string>();
        DirectoryManager::deleteDirectories(currSong, musicRootPath);

        CoverArtManager covMgr(m_bConf);
        covMgr.updateCover(updatedSong, currSong);
    }
}
