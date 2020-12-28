#include "database/SongRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

#include "type/SongFilter.h"

namespace database {
    SongRepository::SongRepository(const model::BinaryPath& bConf) : BaseRepository(bConf) { }


    std::vector<model::Song> SongRepository::retrieveRecords() {
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);
		std::stringstream qry;
		qry << "SELECT sng.*, alb.Artist AS AlbumArtist FROM Song sng ";
		qry << "LEFT JOIN Album alb ON sng.AlbumId=alb.AlbumId";
		const auto query = qry.str();

		::mysql_stmt_prepare(stmt, query.c_str(), query.size());
		::mysql_stmt_execute(stmt);

		auto songs = parseRecords(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		return songs;
    }


    model::Song SongRepository::retrieveRecord(const model::Song& song, type::SongFilter filter) {
		std::stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		auto valueFilterCount = 1;
		switch (filter) {
		    case type::SongFilter::titleAndArtist:
		        valueFilterCount = 2;
		        break;
            case type::SongFilter::titleAlbArtistAlbum:
                valueFilterCount = 3;
		        break;
            case type::SongFilter::titleAlbArtistAlbumTrack:
                valueFilterCount = 4;
                break;
		    default:
				break;
		}

		MYSQL_BIND params[valueFilterCount];
		memset(params, 0, sizeof(params));

		qry << "SELECT sng.*, alb.Artist AS AlbumArtist FROM Song sng ";
		qry << "LEFT JOIN Album alb ON sng.AlbumId=alb.AlbumId WHERE ";

		auto titleLength = song.title.size();
		auto artistLength = song.artist.size();
        auto albumArtistLength = song.albumArtist.size();
        auto albumLength = song.album.size();
		switch (filter) {
		    case type::SongFilter::id:
		        qry << "sng.SongId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&song.id;
		        params[0].length = 0;
		        params[0].is_null = 0;;
		        break;
		    case type::SongFilter::title:
		        qry << "sng.Title = ?";

		        params[0].buffer_type = MYSQL_TYPE_STRING;
		        params[0].buffer = (char*)song.title.c_str();
		        params[0].length = &titleLength;
		        params[0].is_null = 0;
		        break;
		    case type::SongFilter::titleAndArtist:
		        qry << "sng.Title = ? AND sng.Artist = ?";

		        params[0].buffer_type = MYSQL_TYPE_STRING;
		        params[0].buffer = (char*)song.title.c_str();
		        params[0].length = &titleLength;
		        params[0].is_null = 0;

		        params[1].buffer_type = MYSQL_TYPE_STRING;
		        params[1].buffer = (char*)song.artist.c_str();
		        params[1].length = &artistLength;
		        params[1].is_null = 0;

		        std::cout << "title: " << song.title.c_str() << " artist: " << 
		            song.artist.c_str() << "\n";
		        break;
            case type::SongFilter::titleAlbArtistAlbum:
                qry << "sng.Title = ? AND sng.Album = ? AND alb.Artist = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char*)song.title.c_str();
                params[0].length = &titleLength;
                params[0].is_null = 0;

                params[1].buffer_type = MYSQL_TYPE_STRING;
                params[1].buffer = (char*)song.album.c_str();
                params[1].length = &albumLength;
                params[1].is_null = 0;

                params[2].buffer_type = MYSQL_TYPE_STRING;
                params[2].buffer = (char*)song.albumArtist.c_str();
                params[2].length = &albumArtistLength;
                params[2].is_null = 0;
                break;
            case type::SongFilter::titleAlbArtistAlbumTrack:
                qry << "sng.Title = ? AND sng.Album = ? AND alb.Artist = ? AND sng.Track = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char*)song.title.c_str();
                params[0].length = &titleLength;
                params[0].is_null = 0;

                params[1].buffer_type = MYSQL_TYPE_STRING;
                params[1].buffer = (char*)song.album.c_str();
                params[1].length = &titleLength;
                params[1].is_null = 0;

                params[2].buffer_type = MYSQL_TYPE_STRING;
                params[2].buffer = (char*)song.albumArtist.c_str();
                params[2].length = &titleLength;
                params[2].is_null = 0;

		        params[3].buffer_type = MYSQL_TYPE_LONG;
		        params[3].buffer = (char*)&song.track;
		        params[3].length = 0;
		        params[3].is_null = 0;;
                break;
		    default:
		        break;
		}

		qry << " LIMIT 1";

		const auto query = qry.str();
		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		std::cout << "the query has been performed\n";

		auto retrievedSong = parseRecord(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		return retrievedSong;
    }


    bool SongRepository::doesSongExist(const model::Song& song, type::SongFilter filter) {
		std::cout << "checking to see if song exists\n";
		std::stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);
		auto valueFilterCount = 1;
		switch (filter) {
		    case type::SongFilter::titleAndArtist:
		        valueFilterCount = 2;
		        break;
            case type::SongFilter::titleAlbArtistAlbum:
                valueFilterCount = 3;
                break;
            case type::SongFilter::titleAlbArtistAlbumTrack:
                valueFilterCount = 4;
                break;
		    default:
		        break;
		}


		MYSQL_BIND params[valueFilterCount];
		memset(params, 0, sizeof(params));

		qry << "SELECT sng.* FROM Song sng ";
        qry << "LEFT JOIN Album alb ON sng.AlbumId = alb.AlbumId WHERE ";

		auto titleLength = song.title.size();
		auto artistLength = song.artist.size();
        auto albumArtistLength = song.albumArtist.size();
        auto albumLength = song.album.size();
		switch (filter) {
		    case type::SongFilter::id:
		        qry << "sng.SongId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&song.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    case type::SongFilter::title:
		        qry << "sng.Title = ?";

		        params[0].buffer_type = MYSQL_TYPE_STRING;
		        params[0].buffer = (char*)song.title.c_str();
		        params[0].length = &titleLength;
		        params[0].is_null = 0;
		        break;
		    case type::SongFilter::titleAndArtist:
		        qry << "sng.Title = ? AND sng.Artist = ?";

		        params[0].buffer_type = MYSQL_TYPE_STRING;
		        params[0].buffer = (char*)song.title.c_str();
		        params[0].length = &titleLength;
		        params[0].is_null = 0;

		        params[1].buffer_type = MYSQL_TYPE_STRING;
		        params[1].buffer = (char*)song.artist.c_str();
		        params[1].length = &artistLength;
		        params[1].is_null = 0;
		        break;
            case type::SongFilter::titleAlbArtistAlbum:
                qry << "sng.Title = ? AND sng.Album = ? AND alb.Artist = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char*)song.title.c_str();
                params[0].length = &titleLength;
                params[0].is_null = 0;

                params[1].buffer_type = MYSQL_TYPE_STRING;
                params[1].buffer = (char*)song.album.c_str();
                params[1].length = &albumLength;
                params[1].is_null = 0;

                params[1].buffer_type = MYSQL_TYPE_STRING;
                params[1].buffer = (char*)song.albumArtist.c_str();
                params[1].length = &albumArtistLength;
                params[1].is_null = 0;
                break;
            case type::SongFilter::titleAlbArtistAlbumTrack:
                qry << "sng.Title = ? AND sng.Album = ? AND alb.Artist = ? AND sng.Track = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char*)song.title.c_str();
                params[0].length = &titleLength;
                params[0].is_null = 0;

                params[1].buffer_type = MYSQL_TYPE_STRING;
                params[1].buffer = (char*)song.album.c_str();
                params[1].length = &titleLength;
                params[1].is_null = 0;

                params[2].buffer_type = MYSQL_TYPE_STRING;
                params[2].buffer = (char*)song.albumArtist.c_str();
                params[2].length = &titleLength;
                params[2].is_null = 0;

		        params[3].buffer_type = MYSQL_TYPE_LONG;
		        params[3].buffer = (char*)&song.track;
		        params[3].length = 0;
		        params[3].is_null = 0;;
                break;
		    default:
		        break;
		}

		qry << " LIMIT 1";

		const std::string query = qry.str();
		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		std::cout << "the query has been performed\n";

		::mysql_stmt_store_result(stmt);
		auto rowCount = ::mysql_stmt_num_rows(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		return (rowCount > 0) ? true : false;
    }

    bool SongRepository::deleteRecord(const model::Song& song) {
		auto conn = setupMysqlConnection();
		auto status = 0;

		const std::string query("DELETE FROM Song WHERE SongId = " + std::to_string(song.id));

		auto result = performMysqlQuery(conn, query);

		mysql_close(conn);

		return (result == 0) ? true : false;
    }

    void SongRepository::saveRecord(const model::Song& song) {
		std::cout << "beginning to insert song record\n";
		auto conn = setupMysqlConnection();
		auto status = 0;

		MYSQL_STMT *stmt = mysql_stmt_init(conn);

		std::string query = "INSERT INTO Song(Title, Artist, Album, Genre, ";
		query.append("Year, Duration, Track, Disc, SongPath, CoverArtId, ArtistId, ");
		query.append("AlbumId, GenreId, YearId) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

		status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

		MYSQL_BIND params[14];
		memset(params, 0, sizeof(params));

		params[0].buffer_type = MYSQL_TYPE_STRING;
		params[0].buffer = (char*)song.title.c_str();
		auto titleLength = song.title.size();
		params[0].length = &titleLength;
		params[0].is_null = 0;

		params[1].buffer_type = MYSQL_TYPE_STRING;
		params[1].buffer = (char*)song.artist.c_str();
		auto artistLength = song.artist.size();
		params[1].length = &artistLength;
		params[1].is_null = 0;

		params[2].buffer_type = MYSQL_TYPE_STRING;
		params[2].buffer = (char*)song.album.c_str();
		auto albumLength = song.album.size();
		params[2].length = &albumLength;
		params[2].is_null = 0;

		params[3].buffer_type = MYSQL_TYPE_STRING;
		params[3].buffer = (char*)song.genre.c_str();
		auto genreLength = song.genre.size();
		params[3].length = &genreLength;
		params[3].is_null = 0;

		params[4].buffer_type = MYSQL_TYPE_LONG;
		params[4].buffer = (char*)&song.year;
		params[4].length = 0;
		params[4].is_null = 0;

		params[5].buffer_type = MYSQL_TYPE_LONG;
		params[5].buffer = (char*)&song.duration;
		params[5].length = 0;
		params[5].is_null = 0;

		params[6].buffer_type = MYSQL_TYPE_LONG;
		params[6].buffer = (char*)&song.track;
		params[6].length = 0;
		params[6].is_null = 0;

		params[7].buffer_type = MYSQL_TYPE_LONG;
		params[7].buffer = (char*)&song.disc;
		params[7].length = 0;
		params[7].is_null = 0;

		params[8].buffer_type = MYSQL_TYPE_STRING;
		params[8].buffer = (char*)song.songPath.c_str();
		auto pathLength = song.songPath.size();
		params[8].length = &pathLength;
		params[8].is_null = 0;

		params[9].buffer_type = MYSQL_TYPE_LONG;
		params[9].buffer = (char*)&song.coverArtId;
		params[9].length = 0;
		params[9].is_null = 0;

		params[10].buffer_type = MYSQL_TYPE_LONG;
		params[10].buffer = (char*)&song.artistId;
		params[10].length = 0;
		params[10].is_null = 0;

		params[11].buffer_type = MYSQL_TYPE_LONG;
		params[11].buffer = (char*)&song.albumId;
		params[11].length = 0;
		params[11].is_null = 0;
		
		params[12].buffer_type = MYSQL_TYPE_LONG;
		params[12].buffer = (char*)&song.genreId;
		params[12].length = 0;
		params[12].is_null = 0;

		params[13].buffer_type = MYSQL_TYPE_LONG;
		params[13].buffer = (char*)&song.yearId;
		params[13].length = 0;
		params[13].is_null = 0;

		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout << "done inserting song record\n";
    }

    void SongRepository::updateRecord(const model::Song& song) {
		std::cout << "executing query to update record\n";
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		std::stringstream qry;
		qry << "UPDATE Song SET Title = ?, Artist = ?, Album = ?, Genre = ?, ";
		qry << "Year = ?, SongPath = ?, CoverArtId = ?, ArtistId = ?, ";
		qry << "AlbumId = ?, GenreId = ?, YearId = ? WHERE SongId = ?";
		const auto query = qry.str();

		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

		MYSQL_BIND params[12];
		memset(params, 0, sizeof(params));

		params[0].buffer_type = MYSQL_TYPE_STRING;
		params[0].buffer = (char*)song.title.c_str();
		auto titleLength = song.title.size();
		params[0].length = &titleLength;
		params[0].is_null = 0;

		params[1].buffer_type = MYSQL_TYPE_STRING;
		params[1].buffer = (char*)song.artist.c_str();
		auto artistLength = song.artist.size();
		params[1].length = &artistLength;
		params[1].is_null = 0;

		params[2].buffer_type = MYSQL_TYPE_STRING;
		params[2].buffer = (char*)song.album.c_str();
		auto albumLength = song.album.size();
		params[2].length = &albumLength;
		params[2].is_null = 0;

		params[3].buffer_type = MYSQL_TYPE_STRING;
		params[3].buffer = (char*)song.genre.c_str();
		auto genreLength = song.genre.size();
		params[3].length = &genreLength;
		params[3].is_null = 0;

		params[4].buffer_type = MYSQL_TYPE_LONG;
		params[4].buffer = (char*)&song.year;
		params[4].length = 0;
		params[4].is_null = 0;

		params[5].buffer_type = MYSQL_TYPE_STRING;
		params[5].buffer = (char*)song.songPath.c_str();
		auto pathLength = song.songPath.size();
		params[5].length = &pathLength;
		params[5].is_null = 0;

		params[6].buffer_type = MYSQL_TYPE_LONG;
		params[6].buffer = (char*)&song.coverArtId;
		params[6].length = 0;
		params[6].is_null = 0;

		params[7].buffer_type = MYSQL_TYPE_LONG;
		params[7].buffer = (char*)&song.artistId;
		params[7].length = 0;
		params[7].is_null = 0;

		params[8].buffer_type = MYSQL_TYPE_LONG;
		params[8].buffer = (char*)&song.albumId;
		params[8].length = 0;
		params[8].is_null = 0;
		
		params[9].buffer_type = MYSQL_TYPE_LONG;
		params[9].buffer = (char*)&song.genreId;
		params[9].length = 0;
		params[9].is_null = 0;

		params[10].buffer_type = MYSQL_TYPE_LONG;
		params[10].buffer = (char*)&song.yearId;
		params[10].length = 0;
		params[10].is_null = 0;

		params[11].buffer_type = MYSQL_TYPE_LONG;
		params[11].buffer = (char*)&song.id;
		params[11].length = 0;
		params[11].is_null = 0;

		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout << "updated song record\n";
    }


    std::shared_ptr<MYSQL_BIND> SongRepository::valueBind(model::Song& song,
	        std::tuple<char*, char*, char*, char*, char*, char*>& metadata) {
		constexpr auto strLen = 1024;
		constexpr auto valueCount = 16;
		std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(valueCount, sizeof(MYSQL_BIND)));
		unsigned long len[valueCount];
		my_bool nullRes[valueCount];


		values.get()[0].buffer_type = MYSQL_TYPE_LONG;
		values.get()[0].buffer = (char*)&song.id;

		values.get()[1].buffer_type = MYSQL_TYPE_STRING;
		values.get()[1].buffer = (char*)std::get<0>(metadata);
		values.get()[1].buffer_length = strLen;

		values.get()[2].buffer_type = MYSQL_TYPE_STRING;
		values.get()[2].buffer = (char*)std::get<1>(metadata);
		values.get()[2].buffer_length = strLen;

		values.get()[3].buffer_type = MYSQL_TYPE_STRING;
		values.get()[3].buffer = (char*)std::get<2>(metadata);
		values.get()[3].buffer_length = strLen;

		values.get()[4].buffer_type = MYSQL_TYPE_STRING;
		values.get()[4].buffer = (char*)std::get<3>(metadata);
		values.get()[4].buffer_length = strLen;

		values.get()[5].buffer_type = MYSQL_TYPE_LONG;
		values.get()[5].buffer = (char*)&song.year;

		values.get()[6].buffer_type = MYSQL_TYPE_LONG;
		values.get()[6].buffer = (char*)&song.duration;

		values.get()[7].buffer_type = MYSQL_TYPE_LONG;
		values.get()[7].buffer = (char*)&song.track;

		values.get()[8].buffer_type = MYSQL_TYPE_LONG;
		values.get()[8].buffer = (char*)&song.disc;

		values.get()[9].buffer_type = MYSQL_TYPE_STRING;
		values.get()[9].buffer = (char*)std::get<4>(metadata);
		values.get()[9].buffer_length = strLen;

		values.get()[10].buffer_type = MYSQL_TYPE_LONG;
		values.get()[10].buffer = (char*)&song.coverArtId;

		values.get()[11].buffer_type = MYSQL_TYPE_LONG;
		values.get()[11].buffer = (char*)&song.artistId;

		values.get()[12].buffer_type = MYSQL_TYPE_LONG;
		values.get()[12].buffer = (char*)&song.albumId;;

		values.get()[13].buffer_type = MYSQL_TYPE_LONG;
		values.get()[13].buffer = (char*)&song.genreId;

		values.get()[14].buffer_type = MYSQL_TYPE_LONG;
		values.get()[14].buffer = (char*)&song.yearId;

		values.get()[15].buffer_type = MYSQL_TYPE_STRING;
		values.get()[15].buffer = (char*)std::get<5>(metadata);
		values.get()[15].buffer_length = strLen;

		return values;
    }


    std::tuple<char*, char*, char*, char*, char*, char*> SongRepository::metadataBuffer() {
		constexpr auto length = 1024;
		char title[length];
		char artist[length];
		char album[length];
		char genre[length];
		char path[length];
		char albumArtist[length];

		return std::make_tuple(title, artist, album, genre, path, albumArtist);
    }


    std::vector<model::Song> SongRepository::parseRecords(MYSQL_STMT *stmt) {
		::mysql_stmt_store_result(stmt);
		auto c = ::mysql_stmt_num_rows(stmt);
		std::cout << "number of results " << c << "\n";
		std::vector<model::Song> songs;
		songs.reserve(c);

		auto status = 0;
		auto time = 0;

		while (status == 0) {
		    if (::mysql_stmt_field_count(stmt) > 0) {
		        model::Song song;
		        auto metaBuff = metadataBuffer();
		        auto val = valueBind(song, metaBuff);

		        status = ::mysql_stmt_bind_result(stmt, val.get());

		        while (1) {
		            std::cout << "fetching statement result\n";
		            status = ::mysql_stmt_fetch(stmt);

		            if (status == 1 || status == MYSQL_NO_DATA) {
		                break;
		            }
                
		            song.title = std::get<0>(metaBuff);
		            song.artist = std::get<1>(metaBuff);
		            song.album = std::get<2>(metaBuff);
		            song.genre = std::get<3>(metaBuff);
		            song.songPath = std::get<4>(metaBuff);
		            song.albumArtist = std::get<5>(metaBuff);

    		        songs.push_back(song);
		        }
		    }
		    std::cout << "fetching next result\n";
		    status = ::mysql_stmt_next_result(stmt);
		}

		return songs;
    }


    model::Song SongRepository::parseRecord(MYSQL_STMT *stmt) {
		mysql_stmt_store_result(stmt);
		std::cout << "amount of rows: " << mysql_stmt_num_rows(stmt) << "\n";
		model::Song song;
		auto metaBuff = metadataBuffer();
		auto bindedValues = valueBind(song, metaBuff);
		auto status = mysql_stmt_bind_result(stmt, bindedValues.get());
		status = mysql_stmt_fetch(stmt);

		song.title = std::get<0>(metaBuff);
		song.artist = std::get<1>(metaBuff);
		song.album = std::get<2>(metaBuff);
		song.genre = std::get<3>(metaBuff);
		song.songPath = std::get<4>(metaBuff);
		song.albumArtist = std::get<5>(metaBuff);
		
		std::cout << "done parsing record\n";

		return song;
    }
}
