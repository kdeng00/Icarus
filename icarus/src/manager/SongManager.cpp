#include "manager/SongManager.h"

#include <fstream>
#include <filesystem>
#include <random>

#include <icarus_lib/icarus.h>
#include <nlohmann/json.hpp>
#include <oatpp/web/server/api/ApiController.hpp>

#include "database/Repositories.h"
#include "manager/Manager.h"
#include "type/PathType.h"
#include "utility/MetadataRetriever.h"

namespace fs = std::filesystem;

using std::string;
using std::string_view;
using std::pair;
using std::cout;
using std::fstream;
using std::to_string;
using std::map;


using icarus_lib::binary_path;
using icarus_lib::song;
using icarus_lib::album;

using utility::MetadataRetriever;
using type::SongChanged;
using type::SongFilter;

namespace manager {
    SongManager::SongManager(const binary_path& bConf) : m_bConf(bConf)
    {
    }


    pair<bool, type::SongUpload> SongManager::saveSong(song& song)
    {
		saveSongTemp(song);
		MetadataRetriever meta;
		auto data = std::move(song.data);
		song = meta.retrieveMetadata(song);
		song.data = std::move(data);

		database::SongRepository songRepo(m_bConf);
		if (songRepo.doesSongExist(song, SongFilter::titleAlbArtistAlbum)) {
		    cout << "\ntitle: " << song.title << "\nartist: " << song.artist << "\n";
		    cout << "does not exist\n";
		    return std::make_pair(false, type::SongUpload::AlreadyExist);
		}

		saveMisc(song);

		printSong(song);

		songRepo.saveRecord(song);
		song = songRepo.retrieveRecord(song, SongFilter::titleAlbArtistAlbum);

		return std::make_pair(true, type::SongUpload::Successful);
    }


    bool SongManager::didSongChange(const song& updatedSong, 
		    const song& currSong) {
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

    bool SongManager::requiresFilesystemChange(const song& updatedSong, 
		    const song& currSong) {
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

    bool SongManager::deleteSong(song& song) {
		database::SongRepository songRepo(m_bConf);

		if (!songRepo.doesSongExist(song, SongFilter::id)) {
		    cout << "song does not exist\n";
		    return false;
		}

		song = songRepo.retrieveRecord(song, SongFilter::id);

		auto paths = directory_manager::pathConfigContent(m_bConf);

		auto deleted = songRepo.deleteRecord(song);

		if (!deleted) {
		    cout << "song not deleted from databases\n";
		    return deleted;
		}
		deleteMisc(song);

		fs::remove(song.song_path);
		directory_manager::deleteDirectories(song, paths["root_music_path"].get<string>());
		return deleted;
    }

    bool SongManager::updateSong(song& updatedSong) {
		database::SongRepository songRepo(m_bConf);
		song currSong(updatedSong.id);

		currSong = songRepo.retrieveRecord(currSong, SongFilter::id);
		if (!didSongChange(updatedSong, currSong)) {
		    cout << "no change to the song\n";
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


    void SongManager::printSong(const song& song) {
		cout << "\nsong" << "\n";
		cout << "title: " << song.title << "\n";
		cout << "artist: " << song.artist << "\n";
		cout << "album artist: " << song.album_artist << "\n";
		cout << "album: " << song.album << "\n";
		cout << "genre: " << song.genre << "\n";
		cout << "duration: " << song.duration << "\n";
		cout << "year: " << song.year << "\n";
		cout << "track: " << song.track << "\n";
		cout << "disc: " << song.disc << "\n";
		cout << "song path: " << song.song_path << "\n";
		cout << "cover art id: " << song.cover_art_id << "\n";
		cout << "album id: " << song.album_id << "\n";
		cout << "artist id: " << song.artist_id << "\n";
		cout << "genre id: " << song.genre_id << "\n";
		cout << "year id: " << song.year_id << "\n";
    }


    map<SongChanged, bool> SongManager::changesInSong(const song& updatedSong, 
                                                      const song& currSong)
    {
	    map<SongChanged, bool> songChanges;

		string_view updatedTitle = updatedSong.title;
		string_view updatedArtist = updatedSong.artist;
		string_view updatedAlbum = updatedSong.album;
		string_view updatedGenre = updatedSong.genre;

		songChanges[SongChanged::title] = 
				(currSong.title.compare(updatedTitle) != 0 && 
				 updatedTitle.size() > 0) ? true : false;

		songChanges[SongChanged::artist] = 
				(currSong.artist.compare(updatedArtist) != 0 && 
				 updatedArtist.size() > 0) ? true : false;

		songChanges[SongChanged::album] = 
				(currSong.album.compare(updatedAlbum) != 0 && 
				 updatedAlbum.size() > 0) ? true : false;

		songChanges[SongChanged::genre] = 
				(currSong.genre.compare(updatedGenre) != 0 && 
				 updatedGenre.size() > 0) ? true : false;

		songChanges[SongChanged::year] =
				(updatedSong.year != 0) ? true : false;

		return songChanges;
}


    string SongManager::createSongPath(const song& song) {
		auto song_path = directory_manager::createDirectoryProcess(
				song, m_bConf, type::PathType::music);

		if (song.track != 0) {
		    song_path.append("track");
		    auto trackNum = (song.track > 9) ?
				    to_string(song.track) : "0" + to_string(song.track);
		    song_path.append(trackNum);
		} else {
		    song_path.append(song.title);
		}
		song_path.append(".mp3");

		return song_path;
    }


    // used to prevent empty values to appear in the updated song
    void SongManager::assignMiscFields(map<SongChanged, bool>& songChanges, song& updatedSong,
		                               const song& currSong)
    {
		cout << "assigning miscellanes fields to updated song\n";
		updatedSong.track = currSong.track;
		for (auto scIter = songChanges.begin(); scIter != songChanges.end(); ++scIter) {
		    SongChanged key = scIter->first;
		    bool changed = songChanges.at(key);

		    if (!changed) {
                switch (key) {
		            case SongChanged::title:
		                updatedSong.title = currSong.title;
		                cout << "title has not been changed\n";
		                break;
                    case SongChanged::artist:
                        updatedSong.artist = currSong.artist;
                        cout << "artist has not been changed\n";
                        break;
                    case SongChanged::album:
                        updatedSong.album = currSong.album;
                        cout << "album has not been changed\n";
                        break;
                    case SongChanged::genre:
                        updatedSong.genre = currSong.genre;
                        cout << "genre has not been changed\n";
                        break;
                    case::SongChanged::year:
                        updatedSong.year = currSong.year;
                        cout << "year has not been changed\n";
                    default:
                        break;
                }
            }
        }
    }

    // used to dump miscellaneous id to the updated song
    void SongManager::assignMiscId(song& updatedSong,
            const song& currSong) {
        cout << "assigning miscellaneous Id's to updated song\n";
        updatedSong.artist_id = currSong.artist_id;
        updatedSong.album_id = currSong.album_id;
        updatedSong.genre_id = currSong.genre_id;
        updatedSong.year_id = currSong.year_id;
        updatedSong.cover_art_id = currSong.cover_art_id;
    }

    // saves song to a temporary path
    void SongManager::saveSongTemp(song& song) {
        auto config = directory_manager::pathConfigContent(m_bConf);

        auto tmpSongPath = config["temp_root_path"].get<string>();
        std::random_device dev;
        std::mt19937 rng(dev());
        std::uniform_int_distribution<std::mt19937::result_type> dist(1,1000);

        tmpSongPath.append(to_string(dist(rng)));
        tmpSongPath.append(".mp3");

        fstream s(tmpSongPath, fstream::binary | fstream::out);
        s.write((char*)&song.data[0], song.data.size());
        s.close();

        song.song_path = tmpSongPath;
    }

    void SongManager::saveMisc(song& song) {
        cover_art_manager covMgr(m_bConf);
        auto pathConfigContent = directory_manager::pathConfigContent(m_bConf);
        auto musicRootPath = pathConfigContent["root_music_path"].get<string>();

        auto cov = covMgr.saveCover(song);
        const auto song_path = createSongPath(song);

        if (fs::exists(song_path)) {
            cout << "deleting old song with the same metadata\n";
            fs::remove(song_path);
        }
        cout << "copying song to the appropriate directory\n";
        cout << song.song_path << "\n";
        cout << song_path << "\n";
        fs::copy(song.song_path, song_path);
        fs::remove(song.song_path);
        song.song_path = std::move(song_path);
        cout << "copied song to the appropriate directory\n";

        album_manager albMgr(m_bConf);
        auto album = albMgr.saveAlbum(song);
        album = albMgr.retrieveAlbum(album);
        album_manager::printAlbum(album);

        artist_manager artMgr(m_bConf);
        auto artist = artMgr.saveArtist(song);
        artist = artMgr.retrieveArtist(artist);
        artist_manager::printArtist(artist);

        genre_manager gnrMgr(m_bConf);
        auto genre = gnrMgr.saveGenre(song);
        genre = gnrMgr.retrieveGenre(genre);
        genre_manager::printGenre(genre);

        year_manager yrMgr(m_bConf);
        auto year = yrMgr.saveYear(song);
        year = yrMgr.retrieveYear(year);
        year_manager::printYear(year);

        song.cover_art_id = cov.id;
        song.album_id = album.id;
        song.artist_id = artist.id;
        song.genre_id = genre.id;
        song.year_id = year.id;

        cout << "done with miscellaneous database records\n";
    }

    void SongManager::deleteMisc(const song& song) {
        cover_art_manager covMgr(m_bConf);
        covMgr.deleteCover(song);

        album_manager albMgr(m_bConf);
        albMgr.deleteAlbum(song);

        artist_manager artMgr(m_bConf);
        artMgr.deleteArtist(song);

        genre_manager gnrMgr(m_bConf);
        gnrMgr.deleteGenre(song);

        year_manager yrMgr(m_bConf);
        yrMgr.deleteYear(song);
    }

    // deletes miscellanes records
    void SongManager::deleteMiscExceptCoverArt(const song& song) {
        album_manager albMgr(m_bConf);
        albMgr.deleteAlbum(song);

        artist_manager artMgr(m_bConf);
        artMgr.deleteArtist(song);

        genre_manager gnrMgr(m_bConf);
        gnrMgr.deleteGenre(song);

        year_manager yrMgr(m_bConf);
        yrMgr.deleteYear(song);
    }

    void SongManager::updateMisc(const map<SongChanged, bool>& songChanges, song& updatedSong, 
                                 const song& currSong)
{
        auto titleChange = songChanges.at(SongChanged::title);
        auto artistChange = songChanges.at(SongChanged::artist);
        auto albumChange = songChanges.at(SongChanged::album);
        auto genreChange = songChanges.at(SongChanged::genre);
        auto yearChange = songChanges.at(SongChanged::year);

        if (artistChange) {
            artist_manager artMgr(m_bConf);
            artMgr.updateArtist(updatedSong, currSong);
        }
        if (albumChange) {
            album_manager albMgr(m_bConf);
            albMgr.updateAlbum(updatedSong, currSong);
        }
        if (genreChange) {
            genre_manager gnrMgr(m_bConf);
            gnrMgr.updateGenre(updatedSong, currSong);
        }
        if (yearChange) {
            year_manager yrMgr(m_bConf);
            yrMgr.updateYear(updatedSong, currSong);
        }

        // determins to update the cover art record
        if (titleChange || artistChange || albumChange) {
            cover_art_manager covMgr(m_bConf);
            covMgr.updateCoverRecord(updatedSong);
        }

        database::SongRepository songRepo(m_bConf);
        songRepo.updateRecord(updatedSong);

        deleteMiscExceptCoverArt(currSong);
    }

    void SongManager::modifySongOnFilesystem(song& updatedSong, 
            const song& currSong) {
        cout << "preparing to modify song\n";
        auto song_path = createSongPath(updatedSong);
        updatedSong.song_path = std::move(song_path);

        cout << "new path " << updatedSong.song_path << "\n";

        fs::copy(currSong.song_path, updatedSong.song_path);
        fs::remove(currSong.song_path);

        auto paths = directory_manager::pathConfigContent(m_bConf);
        const auto musicRootPath = 
            paths[directory_manager::retrievePathType(
                type::PathType::music)].get<string>();
        directory_manager::deleteDirectories(currSong, musicRootPath);

        cover_art_manager covMgr(m_bConf);
        covMgr.updateCover(updatedSong, currSong);
    }
}
