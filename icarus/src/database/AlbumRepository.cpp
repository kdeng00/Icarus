#include "database/AlbumRepository.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

namespace database {
    AlbumRepository::AlbumRepository(const model::BinaryPath& bConf) : BaseRepository(bConf) { }


    std::vector<model::Album> AlbumRepository::retrieveRecords() {
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        const std::string query = "SELECT * FROM Album";
        mysql_stmt_prepare(stmt, query.c_str(), query.size());
        mysql_stmt_execute(stmt);

        auto albums = parseRecords(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);

        return albums;
    }


    std::pair<model::Album, int> AlbumRepository::retrieveRecordWithSongCount(model::Album& album, 
            type::AlbumFilter filter = type::AlbumFilter::id) {
        std::cout << "retrieving album with song count\n";
        std::stringstream qry;
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        MYSQL_BIND params[1];
        std::memset(params, 0, sizeof(params));

        qry << "SELECT alb.*, COUNT(*) AS SongCount FROM Album alb LEFT JOIN ";
        qry << "Song sng ON alb.AlbumId=sng.AlbumId WHERE ";

        switch (filter) {
            case type::AlbumFilter::id:
                qry << "sng.AlbumId = ?";

                params[0].buffer_type = MYSQL_TYPE_LONG;
                params[0].buffer = (char*)&album.id;
                params[0].length = 0;
                params[0].is_null = 0;
                break;
            default:
                break;
        }
        qry << " GROUP BY alb.AlbumId LIMIT 1";

        const auto query = qry.str();

        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, params);
        status = mysql_stmt_execute(stmt);

        auto albWSC = parseRecordWithSongCount(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);

        return albWSC;
    }

    model::Album AlbumRepository::retrieveRecord(model::Album& album, type::AlbumFilter filter) {
        std::cout << "retrieving album record\n";
        std::stringstream qry;
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        MYSQL_BIND params[1];
        memset(params, 0, sizeof(params));

        qry << "SELECT alb.* FROM Album alb WHERE ";

        auto titleLength = album.title.size();
        switch (filter) {
            case type::AlbumFilter::id:
                qry << "alb.AlbumId = ?";

                params[0].buffer_type = MYSQL_TYPE_LONG;
                params[0].buffer = (char*)&album.id;
                params[0].length = 0;
                params[0].is_null = 0;
                break;
            case type::AlbumFilter::title:
                qry << "alb.Title = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char*)album.title.c_str();
                params[0].length = &titleLength;
                params[0].is_null = 0;
                break;
            default:
                break;
        }

        qry << " LIMIT 1";

        const auto query = qry.str();
        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, params);
        status = mysql_stmt_execute(stmt);

        album = parseRecord(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);

        std::cout << "done\n";

        return album;
    }

    bool AlbumRepository::doesAlbumExists(const model::Album& album, type::AlbumFilter filter) {
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        MYSQL_BIND params[1];
        memset(params, 0, sizeof(params));

        std::stringstream qry;
        qry << "SELECT * FROM Album WHERE ";

        auto titleLength = album.title.size();
        switch (filter) {
            case type::AlbumFilter::id:
                qry << "AlbumId = ?";

                params[0].buffer_type = MYSQL_TYPE_LONG;
                params[0].buffer = (char*)&album.id;
                params[0].length = 0;
                params[0].is_null = 0;
                break;
            case type::AlbumFilter::title:
                qry << "Title = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char*)album.title.c_str();
                params[0].length = &titleLength;
                params[0].is_null = 0;
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

        mysql_stmt_store_result(stmt);
        auto rowCount = mysql_stmt_num_rows(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);

        return (rowCount > 0) ? true : false;
    }

    void AlbumRepository::saveAlbum(const model::Album& album) {
        std::cout << "beginning to insert album record\n";

        auto conn = setupMysqlConnection();
        MYSQL_STMT *stmt = mysql_stmt_init(conn);

        const std::string query = "INSERT INTO Album(Title, Artist, Year) VALUES(?, ?, ?)";

        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

        MYSQL_BIND params[3];
        memset(params, 0, sizeof(params));

        params[0].buffer_type = MYSQL_TYPE_STRING;
		params[0].buffer = (char*)album.title.c_str();
		auto titleLength = album.title.size();
		params[0].length= &titleLength;
		params[0].is_null = 0;

		params[1].buffer_type = MYSQL_TYPE_STRING;
		params[1].buffer = (char*)album.artist.c_str();
		auto artistLength = album.artist.size();
		params[1].length = &artistLength;
		params[1].is_null = 0;

		params[2].buffer_type = MYSQL_TYPE_LONG;
		params[2].buffer = (char*)&album.year;
		params[2].length = 0;
		params[2].is_null = 0;

		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout << "done inserting album record\n";
    }

    void AlbumRepository::deleteAlbum(const model::Album& album, 
            type::AlbumFilter filter = type::AlbumFilter::id) {
		std::cout << "deleting album record\n";

		std::stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		MYSQL_BIND params[1];
		std::memset(params, 0, sizeof(params));

		qry << "DELETE FROM Album WHERE ";

		switch (filter) {
		    case type::AlbumFilter::id:
		        qry << "AlbumId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&album.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    default:
		        break;
		}

		const auto query = qry.str();

		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		std::cout << "execute delete query\n";
    }


    std::vector<model::Album> AlbumRepository::parseRecords(MYSQL_STMT* stmt) {
		std::cout << "parsing album record\n";
		mysql_stmt_store_result(stmt);

		std::vector<model::Album> albums;
		albums.reserve(mysql_stmt_num_rows(stmt));

		const auto valAmt = 3;
		unsigned long len[valAmt];
		my_bool nullRes[valAmt];

		for (auto status = 0; status == 0; status = mysql_stmt_next_result(stmt)) {
		    if (mysql_stmt_field_count(stmt) > 0) {
		        model::Album alb;
		        auto metaBuff = metadataBuffer();
		        auto bindedValues = valueBind(alb, metaBuff);
		        status = mysql_stmt_bind_result(stmt, bindedValues.get());

		        while (true) {
		            std::cout << "fetching statement result\n";
		            status = mysql_stmt_fetch(stmt);

		            if (status == 1 || status == MYSQL_NO_DATA) break;

    		        alb.title = std::get<0>(metaBuff);
		            alb.artist = std::get<1>(metaBuff);
		            albums.push_back(std::move(alb));
		        }
		    }
    		std::cout << "fetching next result\n";
        }

		return albums;
    }


    std::pair<model::Album, int> AlbumRepository::parseRecordWithSongCount(MYSQL_STMT *stmt) {
		std::cout << "parsing album record with song count\n";
		mysql_stmt_store_result(stmt);
		auto rowCount = mysql_stmt_num_rows(stmt);

		model::Album album;
		auto metaBuff = metadataBuffer();
		int songCount = 0;
		auto val = valueBindWithSongCount(album, metaBuff, songCount);

		if (rowCount == 0) {
		    std::cout << "no results\n";
		    return std::make_pair(album, songCount);
		}

		auto status = mysql_stmt_bind_result(stmt, val.get());
		status = mysql_stmt_fetch(stmt);

		album.title = std::get<0>(metaBuff);
		album.artist = std::get<1>(metaBuff);

		std::cout << "done parsing album record with song count\n";

		auto albWSC = std::make_pair(album, songCount);

		return albWSC;
    }


    std::shared_ptr<MYSQL_BIND> AlbumRepository::valueBind(model::Album& album, 
            std::tuple<char*, char*>& metadata) {
	    constexpr auto wordLen = 1024;
	    constexpr auto valueCount = 4;
		std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(valueCount, sizeof(MYSQL_BIND)));

		values.get()[0].buffer_type = MYSQL_TYPE_LONG;
		values.get()[0].buffer = (char*)&album.id;

		values.get()[1].buffer_type = MYSQL_TYPE_STRING;
		values.get()[1].buffer = (char*)std::get<0>(metadata);
		values.get()[1].buffer_length = wordLen;

		values.get()[2].buffer_type = MYSQL_TYPE_STRING;
		values.get()[2].buffer = (char*)std::get<1>(metadata);
		values.get()[2].buffer_length = wordLen;

		values.get()[3].buffer_type = MYSQL_TYPE_LONG;
		values.get()[3].buffer = (char*)&album.year;

		return values;
    }

    std::shared_ptr<MYSQL_BIND> AlbumRepository::valueBindWithSongCount(model::Album& album, 
            std::tuple<char*, char*>& metadata, int& songCount) {
        constexpr auto wordLen = 1024;
        constexpr auto valueCount = 5;
        std::shared_ptr<MYSQL_BIND> values(
            (MYSQL_BIND*) std::calloc(valueCount, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = (char*)&album.id;

        values.get()[1].buffer_type = MYSQL_TYPE_STRING;
        values.get()[1].buffer = (char*)std::get<0>(metadata);
        values.get()[1].buffer_length = wordLen;

        values.get()[2].buffer_type = MYSQL_TYPE_STRING;
        values.get()[2].buffer = (char*)std::get<1>(metadata);
        values.get()[2].buffer_length = wordLen;

        values.get()[3].buffer_type = MYSQL_TYPE_LONG;
        values.get()[3].buffer = (char*)&album.year;

        values.get()[4].buffer_type = MYSQL_TYPE_LONG;
        values.get()[4].buffer = (char*)&songCount;

        return values;
    }


    std::tuple<char*, char*> AlbumRepository::metadataBuffer() {
        constexpr auto wordLen = 1024;
        char title[wordLen];
        char artist[wordLen];

        return std::make_tuple(title, artist);
    }


    model::Album AlbumRepository::parseRecord(MYSQL_STMT *stmt)
    {
        std::cout << "parsing album record\n";
        mysql_stmt_store_result(stmt);
        auto rows = mysql_stmt_num_rows(stmt);

        model::Album album;
        auto metaBuff = metadataBuffer();
        auto bindedValues = valueBind(album, metaBuff);
        auto status = mysql_stmt_bind_result(stmt, bindedValues.get());
        status = mysql_stmt_fetch(stmt);
    
        album.title = std::get<0>(metaBuff);
        album.artist = std::get<1>(metaBuff);

        std::cout << "done parsing album record\n";

        return album;
    }
}
