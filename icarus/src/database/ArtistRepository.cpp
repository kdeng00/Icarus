#include "database/ArtistRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

namespace database {
    ArtistRepository::ArtistRepository(const icarus_lib::binary_path & binConf) : 
            BaseRepository(binConf) { }


    std::vector<icarus_lib::artist> ArtistRepository::retrieveRecords() {
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);
        const std::string query = "SELECT * FROM Artist";

        mysql_stmt_prepare(stmt, query.c_str(), query.size());
        mysql_stmt_execute(stmt);

        auto artists = parseRecords(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);

        return artists;
    }

    std::pair<icarus_lib::artist, int> ArtistRepository::retrieveRecordWithSongCount(
            icarus_lib::artist& artist, type::ArtistFilter filter = type::ArtistFilter::id) {
		std::cout << "retrieving artist record with song count\n";
		std::stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		qry << "SELECT art.*, COUNT(*) AS SongCount FROM Artist art LEFT JOIN ";
		qry << "Song sng ON art.ArtistId=sng.ArtistId WHERE ";

		MYSQL_BIND params[1];
		std::memset(params, 0, sizeof(params));
		
		switch (filter) {
		    case type::ArtistFilter::id:
		        qry << "sng.ArtistId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&artist.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    default:
		        break;
		}

		qry << " GROUP BY art.ArtistId LIMIT 1";

		const auto query = qry.str();
		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		std::cout << "Artist record with song count query ";
		std::cout << "has been performed\n";

		auto artWSC = parseRecordWithSongCount(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		return artWSC;
    }

    icarus_lib::artist ArtistRepository::retrieveRecord(icarus_lib::artist& artist, 
            type::ArtistFilter filter) {
		std::cout << "retrieving artist record\n";
		std::stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);
		qry << "SELECT art.* FROM Artist art WHERE ";

		MYSQL_BIND params[1];
		memset(params, 0, sizeof(params));

		auto artistLength = artist.name.size();
		switch (filter) {
		    case type::ArtistFilter::id:
		        qry << "art.ArtistId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&artist.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    case type::ArtistFilter::artist:
		        qry << "art.Artist = ?";

		        params[0].buffer_type = MYSQL_TYPE_STRING;
		        params[0].buffer = (char*)artist.name.c_str();
		        params[0].length = &artistLength;
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

		artist = parseRecord(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout << "retrieved record\n";

		return artist;
    }

    bool ArtistRepository::doesArtistExist(const icarus_lib::artist& artist, type::ArtistFilter filter) {
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		MYSQL_BIND params[1];
		memset(params, 0, sizeof(params));

		std::stringstream qry;
		qry << "SELECT * FROM Artist WHERE ";

		auto artistLength = artist.name.size();
		switch (filter) {
		    case type::ArtistFilter::id:
		        qry << "ArtistId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&artist.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    case type::ArtistFilter::artist:
		        qry << "Artist = ?";

		        params[0].buffer_type = MYSQL_TYPE_STRING;
		        params[0].buffer = (char*)artist.name.c_str();
		        params[0].length = &artistLength;
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

    void ArtistRepository::saveRecord(const icarus_lib::artist& artist) {
		std::cout << "inserting artist record\n";

		auto conn = setupMysqlConnection();
		MYSQL_STMT *stmt = mysql_stmt_init(conn);

		const std::string query = "INSERT INTO Artist(Artist) VALUES(?)";

		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

		MYSQL_BIND params[1];
		memset(params, 0, sizeof(params));

		params[0].buffer_type = MYSQL_TYPE_STRING;
		params[0].buffer = (char*)artist.name.c_str();
		auto artistLength = artist.name.size();
		params[0].length = &artistLength;
		params[0].is_null = 0;

		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout<< "inserted artist record\n";
    }

    void ArtistRepository::deleteArtist(const icarus_lib::artist& artist, 
            type::ArtistFilter filter = type::ArtistFilter::id) {
		std::cout << "delete Artist record\n";
		std::stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		qry << "DELETE FROM Artist WHERE ";

		MYSQL_BIND params[1];
		std::memset(params, 0, sizeof(params));

		switch (filter) {
		    case type::ArtistFilter::id:
		        qry << "ArtistId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&artist.id;
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

		mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout << "deleted artist record\n";
    }

    std::vector<icarus_lib::artist> ArtistRepository::parseRecords(MYSQL_STMT *stmt) {
		mysql_stmt_store_result(stmt);

		std::vector<icarus_lib::artist> artists;
		artists.reserve(mysql_stmt_num_rows(stmt));

		constexpr auto valAmt = 2;
		unsigned long len[valAmt];
		my_bool nullRes[valAmt];

        for (auto status = 0; status == 0; status = mysql_stmt_next_result(stmt)) {
            if (mysql_stmt_field_count(stmt) > 0) {
                icarus_lib::artist art;
                auto metaBuff = metadataBuffer();
                auto bindedValues = valueBind(art, metaBuff);
                status = mysql_stmt_bind_result(stmt, bindedValues.get());

                std::cout << "fetching statement result\n";
                status = mysql_stmt_fetch(stmt);

                if (status == 1 || status == MYSQL_NO_DATA) break;

                art.name = std::get<0>(metaBuff);
                artists.push_back(art);
            }
            std::cout << "fetching next result\n";
        }

		return artists;
    }


    std::pair<icarus_lib::artist, int> ArtistRepository::parseRecordWithSongCount(MYSQL_STMT *stmt) {
		std::cout << "parsing artist record with song count\n";
		mysql_stmt_store_result(stmt);
        const auto rowCount = mysql_stmt_num_rows(stmt);

        icarus_lib::artist artist;

        auto metaBuff = metadataBuffer();
        auto songCount = 0;
        auto val = valueBindWithSongCount(artist, metaBuff, songCount);

        if (rowCount == 0) {
            std::cout << "no results\n";
            return std::make_pair(artist, songCount);
        }

        auto status = mysql_stmt_bind_result(stmt, val.get());
        status = mysql_stmt_fetch(stmt);

        artist.name = std::get<0>(metaBuff);

        std::cout << "done parsing album record with song count\n";

		return std::make_pair(artist, songCount);
    }


    std::shared_ptr<MYSQL_BIND> ArtistRepository::valueBind(icarus_lib::artist& artist,
            std::tuple<char*>& metadata) {
        constexpr auto valueCount = 2;
        constexpr auto wordLen = 1024;
        std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) 
                std::calloc(valueCount, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = (char*)&artist.id;

        values.get()[1].buffer_type = MYSQL_TYPE_STRING;
        values.get()[1].buffer = (char*)std::get<0>(metadata);
        values.get()[1].buffer_length = wordLen;

        return values;
    }

    std::shared_ptr<MYSQL_BIND> ArtistRepository::valueBindWithSongCount(icarus_lib::artist& artist,
            std::tuple<char*>& metadata, int& songCount) {
        constexpr auto valueCount = 3;
        constexpr auto wordLen = 1024;
        std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) 
                std::calloc(valueCount, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = reinterpret_cast<char*>(&artist.id);

        values.get()[1].buffer_type = MYSQL_TYPE_STRING;
        values.get()[1].buffer = reinterpret_cast<char*>(std::get<0>(metadata));
        values.get()[1].buffer_length = wordLen;

        values.get()[2].buffer_type = MYSQL_TYPE_LONG;
        values.get()[2].buffer = reinterpret_cast<char*>(&songCount);


        return values;
    }


    std::tuple<char*> ArtistRepository::metadataBuffer() {
        constexpr auto wordLen = 1024;
        char artist[wordLen];

        return std::make_tuple(artist);
    }


    icarus_lib::artist ArtistRepository::parseRecord(MYSQL_STMT *stmt) {
		std::cout << "parsing artist record\n";
		mysql_stmt_store_result(stmt);

		icarus_lib::artist art;
        auto metaBuff = metadataBuffer();
        auto bindedValues = valueBind(art, metaBuff);
        auto status = mysql_stmt_bind_result(stmt, bindedValues.get());
        status = mysql_stmt_fetch(stmt);

        art.name = std::get<0>(metaBuff);

		std::cout << "done parsing artist record\n";

		return art;
    }
}
