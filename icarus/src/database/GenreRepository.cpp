#include "database/GenreRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

namespace database {
    GenreRepository::GenreRepository(const icarus_lib::binary_path & bConf) : BaseRepository(bConf) { }


    std::vector<icarus_lib::genre> GenreRepository::retrieveRecords() {
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);
		const std::string query = "SELECT * FROM Genre";

		mysql_stmt_prepare(stmt, query.c_str(), query.size());
		mysql_stmt_execute(stmt);

		auto genres = parseRecords(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		return genres;
    }

    std::pair<icarus_lib::genre, int> GenreRepository::retrieveRecordWithSongCount(
            icarus_lib::genre& genre, type::GenreFilter filter = type::GenreFilter::id) {
		std::cout << "retrieving genre record with song count\n";
		std::stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		qry << "SELECT gnr.*, COUNT(*) AS SongCount FROM Genre gnr LEFT JOIN ";
		qry << "Song sng ON gnr.GenreId=sng.GenreId WHERE ";

		MYSQL_BIND params[1];
		std::memset(params, 0, sizeof(params));

		switch (filter) {
		    case type::GenreFilter::id:
		        qry << "sng.GenreId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&genre.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    default:
		        break;
		}

		qry << " GROUP BY gnr.GenreId LIMIT 1";

		const auto query = qry.str();

		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		auto gnrWSC = parseRecordWithSongCount(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout << "retrieved genre record with song count\n";

		return gnrWSC;
    }

    icarus_lib::genre GenreRepository::retrieveRecord(icarus_lib::genre& genre, type::GenreFilter filter) {
		std::cout << "retrieving genre record\n";
		std::stringstream qry;
		auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);
		qry << "SELECT gnr.* FROM Genre gnr WHERE ";

        MYSQL_BIND params[1];
        std::memset(params, 0, sizeof(params));
        auto categoryLength = genre.category.size();
		
		switch (filter) {
		    case type::GenreFilter::id:
		        qry << "gnr.GenreId = ?";

                params[0].buffer_type = MYSQL_TYPE_LONG;
                params[0].buffer = reinterpret_cast<char*>(&genre.id);
                params[0].length = 0;
                params[0].is_null = 0;
		        break;
		    case type::GenreFilter::category:
		        qry << "gnr.Category = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = const_cast<char*>(genre.category.c_str());
                params[0].length = &categoryLength;
                params[0].is_null = 0;
		        break;
		    default:
		        break;
		}

		qry << " ORDER BY GenreId DESC LIMIT 1";

		const auto query = qry.str();
        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, params);
        status = mysql_stmt_execute(stmt);

		genre = parseRecord(stmt);

        mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout << "retrieved record\n";

		return genre;
    }

    bool GenreRepository::doesGenreExist(const icarus_lib::genre& genre, type::GenreFilter filter) {
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		std::stringstream qry;
		qry << "SELECT * FROM Genre WHERE ";

		MYSQL_BIND params[1];
		memset(params, 0, sizeof(params));

		auto categoryLength = genre.category.size();
		switch (filter) {
		    case type::GenreFilter::id:
		        qry << "GenreId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&genre.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    case type::GenreFilter::category:
		        qry << "Category = ?";

		        params[0].buffer_type = MYSQL_TYPE_STRING;
		        params[0].buffer = (char*)genre.category.c_str();
		        params[0].length = &categoryLength;
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

    void GenreRepository::saveRecord(const icarus_lib::genre& genre) {
		std::cout << "inserting genre record\n";

		auto conn = setupMysqlConnection();
		MYSQL_STMT *stmt = mysql_stmt_init(conn);

		const std::string query("INSERT INTO Genre(Category) VALUES(?)");

		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

		MYSQL_BIND params[1];
		memset(params, 0, sizeof(params));

		params[0].buffer_type = MYSQL_TYPE_STRING;
		params[0].buffer = (char*)genre.category.c_str();
		auto categoryLength = genre.category.size();
		params[0].length = &categoryLength;
		params[0].is_null = 0;

		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		std::cout << "inserted record\n";
    }

    void GenreRepository::deleteRecord(const icarus_lib::genre& genre, 
            type::GenreFilter filter = type::GenreFilter::id) {
		std::cout << "deleting genre record\n";
		std::stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		qry << "DELETE FROM Genre WHERE ";

		MYSQL_BIND params[1];
		std::memset(params, 0, sizeof(params));

		switch (filter) {
		    case type::GenreFilter::id:
		        qry << "GenreId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&genre.id;
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

		std::cout << "deleted genre record\n";
    }


    std::vector<icarus_lib::genre> GenreRepository::parseRecords(MYSQL_STMT *stmt) {
		mysql_stmt_store_result(stmt);

		std::vector<icarus_lib::genre> genres;
		genres.reserve(mysql_stmt_num_rows(stmt));

		constexpr auto valAmt = 2;

		for (auto status = 0; status == 0; status = mysql_stmt_next_result(stmt)) {
            if (mysql_stmt_field_count(stmt) > 0) {
                icarus_lib::genre genre;
                auto metaBuff = metadataBuffer();
                auto bindedValues = valueBind(genre, metaBuff);
                status = mysql_stmt_bind_result(stmt, bindedValues.get());

                std::cout << "fetching statement result\n";
                status = mysql_stmt_fetch(stmt);

                if (status == 1 || status == MYSQL_NO_DATA) break;

                genre.category = std::get<0>(metaBuff);
                genres.push_back(genre);
            }
		    std::cout << "fetching statement result\n";
		}

		return genres;
    }

    std::pair<icarus_lib::genre, int> GenreRepository::parseRecordWithSongCount(MYSQL_STMT *stmt) {
		std::cout << "parsing genre record with song count\n";
		mysql_stmt_store_result(stmt);
        const auto rowCount = mysql_stmt_num_rows(stmt);

		icarus_lib::genre genre;
		int songCount = 0;

        auto metaBuff = metadataBuffer();
        auto val = valueBindWithSongCount(genre, metaBuff, songCount);

        if (rowCount == 0) {
            std::cout << "no results\n";
            return std::make_pair(genre, songCount);
        }

		auto status = mysql_stmt_bind_result(stmt, val.get());
		status = mysql_stmt_fetch(stmt);

		genre.category = std::get<0>(metaBuff);

		std::cout << "parsed genre record with song count\n";

		return std::make_pair(genre, songCount);
    }


    std::shared_ptr<MYSQL_BIND> GenreRepository::valueBind(icarus_lib::genre& genre,
            std::tuple<char*>& metadata) {
        constexpr auto valueCount = 2;
        constexpr auto wordLen = 1024;
        std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*)
                std::calloc(valueCount, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = reinterpret_cast<char*>(&genre.id);

        values.get()[1].buffer_type = MYSQL_TYPE_STRING;
        values.get()[1].buffer = reinterpret_cast<char*>(std::get<0>(metadata));
        values.get()[1].buffer_length = wordLen;

        return values;
    }
    
    std::shared_ptr<MYSQL_BIND> GenreRepository::valueBindWithSongCount(icarus_lib::genre& genre,
            std::tuple<char*>& metadata, int& songCount) {
        constexpr auto valueCount = 3;
        constexpr auto wordLen = 1024;
        std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*)
                std::calloc(valueCount, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = reinterpret_cast<char*>(&genre.id);

        values.get()[1].buffer_type = MYSQL_TYPE_STRING;
        values.get()[1].buffer = reinterpret_cast<char*>(std::get<0>(metadata));
        values.get()[1].buffer_length = wordLen;

        values.get()[2].buffer_type = MYSQL_TYPE_LONG;
        values.get()[2].buffer = reinterpret_cast<char*>(&songCount);

        return values;
    }


    std::tuple<char*> GenreRepository::metadataBuffer() {
        constexpr auto wordLen = 1024;
        char category[wordLen];

        return std::make_tuple(category);
    }


    icarus_lib::genre GenreRepository::parseRecord(MYSQL_STMT *stmt) {
        std::cout << "parsing genre record\n";
        mysql_stmt_store_result(stmt);

		icarus_lib::genre genre;
        auto metaBuff = metadataBuffer();
        auto bindedValues = valueBind(genre, metaBuff);
        auto status = mysql_stmt_bind_result(stmt, bindedValues.get());
        status = mysql_stmt_fetch(stmt);

        genre.category = std::get<0>(metaBuff);

        std::cout << "done parsing genre record\n";

		return genre;
    }
}
