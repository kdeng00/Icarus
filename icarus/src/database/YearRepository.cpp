#include "database/YearRepository.h"

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

using std::string;
using std::stringstream;
using std::vector;
using std::pair;
using std::shared_ptr;
using std::cout;

using icarus_lib::year;
using icarus_lib::binary_path;

using type::YearFilter;

namespace database {
    YearRepository::YearRepository(const binary_path &bConf) : BaseRepository(bConf)
    {
    }


    vector<year> YearRepository::retrieveRecords()
    {
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);
		const string query = "SELECT * FROM Year";

		mysql_stmt_prepare(stmt, query.c_str(), query.size());
		mysql_stmt_execute(stmt);

		auto yearRecs = parseRecords(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		return yearRecs;
    }

    pair<year, int> YearRepository::retrieveRecordWithSongCount(year& year, YearFilter filter)
    {
		cout << "retrieving year record with song count\n";
		stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		qry << "SELECT yr.*, COUNT(*) AS SongCount FROM Year yr LEFT JOIN ";
		qry << "Song sng ON yr.YearId=sng.YearId WHERE ";

		MYSQL_BIND params[1];
		std::memset(params, 0, sizeof(params));

		switch (filter) {
		    case YearFilter::id:
		        qry << "sng.YearId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&year.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    default:
		        break;
		}

		qry << " GROUP BY yr.YearId LIMIT 1";

		const auto query = qry.str();

		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		auto yearWSC = parseRecordWithSongCount(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		cout << "retrieved year record with song count\n";

		return yearWSC;
    }

    year YearRepository::retrieveRecord(year& year, YearFilter filter)
    {
		cout << "retrieving year record\n";
		stringstream qry;
		auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);
		qry << "SELECT yr.* FROM Year yr WHERE ";

        MYSQL_BIND params[1];
        std::memset(params, 0, sizeof(0));

		switch (filter) {
		    case YearFilter::id:
		        qry << "yr.YearId = ?";

                params[0].buffer_type = MYSQL_TYPE_LONG;
                params[0].buffer = reinterpret_cast<char*>(&year.id);
                params[0].length = 0;
                params[0].is_null = 0;
		        break;
		    case YearFilter::year:
		        qry << "yr.Year = ?";

                params[0].buffer_type = MYSQL_TYPE_LONG;
                params[0].buffer = reinterpret_cast<char*>(&year.song_year);
                params[0].length = 0;
                params[0].is_null = 0;
		        break;
		    default:
				    break;
		}

		qry << " ORDER BY yr.YearId DESC LIMIT 1";

		const auto query = qry.str();

        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, params);
        status = mysql_stmt_execute(stmt);

		year = parseRecord(stmt);

        mysql_stmt_close(stmt);
		mysql_close(conn);

		cout << "retrieved record\n";

		return year;
    }


    bool YearRepository::doesYearExist(const year& year, YearFilter filter)
    {
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		stringstream qry;
		qry << "SELECT * FROM Year WHERE ";

		MYSQL_BIND params[1];
		memset(params, 0, sizeof(params));

		switch (filter) {
		    case YearFilter::id:
		        qry << "YearId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&year.id;
		        params[0].length = 0;
		        params[0].is_null = 0;
		        break;
		    case YearFilter::year:
		        qry << "Year = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&year.song_year;
		        params[0].length = 0;
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

		cout << "the query has been performed\n";

		mysql_stmt_store_result(stmt);
		auto rowCount = mysql_stmt_num_rows(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		return (rowCount > 0) ? true : false;
    }


    void YearRepository::saveRecord(const year& year)
    {
		cout << "saving year record\n";

		auto conn = setupMysqlConnection();
		MYSQL_STMT *stmt = mysql_stmt_init(conn);

		const string query("INSERT INTO Year(Year) VALUES(?)");

		auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

		MYSQL_BIND params[1];
		memset(params, 0 , sizeof(params));

		params[0].buffer_type = MYSQL_TYPE_LONG;
		params[0].buffer = (char*)&year.song_year;
		params[0].length = 0;
		params[0].is_null = 0;

		status = mysql_stmt_bind_param(stmt, params);
		status = mysql_stmt_execute(stmt);

		mysql_stmt_close(stmt);
		mysql_close(conn);

		cout << "saved record\n";
    }

    void YearRepository::deleteYear(const year& year, YearFilter filter)
    {
		cout << "deleting year record\n";
		stringstream qry;
		auto conn = setupMysqlConnection();
		auto stmt = mysql_stmt_init(conn);

		qry << "DELETE FROM Year WHERE ";

		MYSQL_BIND params[1];
		std::memset(params, 0, sizeof(params));

		switch (filter) {
		    case YearFilter::id:
		        qry << "YearId = ?";

		        params[0].buffer_type = MYSQL_TYPE_LONG;
		        params[0].buffer = (char*)&year.id;
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

		cout << "deleted year record\n";
    }


    vector<year> YearRepository::parseRecords(MYSQL_STMT *stmt)
    {
		mysql_stmt_store_result(stmt);
        const auto rowCount = mysql_stmt_num_rows(stmt);

		vector<year> yearRecs;
		yearRecs.reserve(rowCount);

		if (mysql_stmt_field_count(stmt) == 0) 
        {
		    cout << "field count is 0\n";
		    return yearRecs;
		}

		const auto valAmt = 2;
		unsigned long len[valAmt];
		my_bool nullRes[valAmt];

		for (auto status = 0; status == 0; status = mysql_stmt_next_result(stmt))
        {
            if (mysql_stmt_field_count(stmt) > 0)
            {
		        year yearRec;

		        cout << "fetching statement result\n";
                auto bindedValues = valueBind(yearRec);
                status = mysql_stmt_bind_result(stmt, bindedValues.get());
		        status = mysql_stmt_fetch(stmt);

                if (status == 1 || status == MYSQL_NO_DATA) break;

		        yearRecs.push_back(yearRec);
            }
            cout << "fetching next result\n";
		}

		return yearRecs;
    }

    pair<year, int> YearRepository::parseRecordWithSongCount(MYSQL_STMT *stmt)
    {
		cout << "parsing year record\n";
		mysql_stmt_store_result(stmt);

		year year;
		int songCount = 0;

		if (mysql_stmt_num_rows(stmt) == 0)
        {
		    cout << "no results\n";
		    return std::make_pair(year, songCount);
		}

        auto val = valueBindWithSongCount(year, songCount);

		auto status = mysql_stmt_bind_result(stmt, val.get());
		status = mysql_stmt_fetch(stmt);

		cout << "parsed year record from the database\n";

		return std::make_pair(year, songCount);
    }


    shared_ptr<MYSQL_BIND> YearRepository::valueBind(year& year)
    {
        constexpr auto valueCount = 2;
        shared_ptr<MYSQL_BIND> values((MYSQL_BIND*)
                std::calloc(valueCount, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = reinterpret_cast<char*>(&year.id);

        values.get()[1].buffer_type = MYSQL_TYPE_LONG;
        values.get()[1].buffer = reinterpret_cast<char*>(&year.song_year);

        return values;
    }

    shared_ptr<MYSQL_BIND> YearRepository::valueBindWithSongCount(year& year,
            int& songCount) {
        constexpr auto valueCount = 3;
        shared_ptr<MYSQL_BIND> values((MYSQL_BIND*)
                std::calloc(valueCount, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = reinterpret_cast<char*>(&year.id);

        values.get()[1].buffer_type = MYSQL_TYPE_LONG;
        values.get()[1].buffer = reinterpret_cast<char*>(&year.song_year);

        values.get()[2].buffer_type = MYSQL_TYPE_LONG;
        values.get()[2].buffer = reinterpret_cast<char*>(&songCount);

        return values;
    }


    year YearRepository::parseRecord(MYSQL_STMT *stmt) {
        cout << "parsing year record\n";
        mysql_stmt_store_result(stmt);

		year year;
        auto bindedValues = valueBind(year);
        auto status = mysql_stmt_bind_result(stmt, bindedValues.get());
        status = mysql_stmt_fetch(stmt);

        cout << "done parsing year record\n";

		return year;
    }
}
