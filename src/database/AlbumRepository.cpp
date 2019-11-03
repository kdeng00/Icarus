#include "database/AlbumRepository.h"

#include <iostream>
#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

namespace database {
AlbumRepository::AlbumRepository(const model::BinaryPath& bConf)
    : BaseRepository(bConf)
{ }


std::vector<model::Album> AlbumRepository::retrieveRecords()
{
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


std::pair<model::Album, int> AlbumRepository::retrieveRecordWithSongCount(model::Album& album, type::AlbumFilter filter = type::AlbumFilter::id)
{
    std::cout << "retrieving album with song count" << std::endl;
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    MYSQL_BIND params[4];
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

model::Album AlbumRepository::retrieveRecord(model::Album& album, type::AlbumFilter filter)
{
    std::cout << "retrieving album record" << std::endl;
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

    std::cout << "done" << std::endl;

    return album;
}

bool AlbumRepository::doesAlbumExists(const model::Album& album, type::AlbumFilter filter)
{
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

    std::cout << "the query has been performed" << std::endl;

    ::mysql_stmt_store_result(stmt);
    auto rowCount = ::mysql_stmt_num_rows(stmt);

    ::mysql_stmt_close(stmt);
    ::mysql_close(conn);

    return (rowCount > 0) ? true : false;
}

void AlbumRepository::saveAlbum(const model::Album& album)
{
    std::cout << "beginning to insert album record" << std::endl;

    auto conn = setupMysqlConnection();
    MYSQL_STMT *stmt = mysql_stmt_init(conn);

    const std::string query = "INSERT INTO Album(Title, Year) VALUES(?, ?)";

    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

    MYSQL_BIND params[2];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)album.title.c_str();
    auto titleLength = album.title.size();
    params[0].length= &titleLength;
    params[0].is_null = 0;

    params[1].buffer_type = MYSQL_TYPE_LONG;
    params[1].buffer = (char*)&album.year;
    params[1].length = 0;
    params[1].is_null = 0;

    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    std::cout << "done inserting album record" << std::endl;
}

void AlbumRepository::deleteAlbum(const model::Album& album, type::AlbumFilter filter = type::AlbumFilter::id)
{
    std::cout << "deleting album record" << std::endl;

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

    std::cout << "execute delete query" << std::endl;
}


std::vector<model::Album> AlbumRepository::parseRecords(MYSQL_STMT* stmt)
{
    std::cout << "parsing album record" << std::endl;
    mysql_stmt_store_result(stmt);

    std::vector<model::Album> albums;
    albums.reserve(mysql_stmt_num_rows(stmt));

    const auto valAmt = 3;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    for (auto status = 0; status == 0; status = mysql_stmt_next_result(stmt)) {
        if (mysql_stmt_field_count(stmt) > 0) {
            model::Album alb;
            auto res = mysql_stmt_result_metadata(stmt);
            auto fields = mysql_fetch_fields(res);
            const auto strLen = 1024;

            MYSQL_BIND val[valAmt];
            memset(val, 0, sizeof(val));

            char title[strLen];

            val[0].buffer_type = MYSQL_TYPE_LONG;
            val[0].buffer = (char*)&alb.id;
            val[0].length = &len[0];
            val[0].is_null = &nullRes[0];

            val[1].buffer_type = MYSQL_TYPE_STRING;
            val[1].buffer = (char*)title;
            val[1].buffer_length = strLen;
            val[1].length = &len[1];
            val[1].is_null = &nullRes[1];

            val[2].buffer_type = MYSQL_TYPE_LONG;
            val[2].buffer = (char*)&alb.year;
            val[2].length = &len[2];
            val[2].is_null = &nullRes[2];

            status = mysql_stmt_bind_result(stmt, val);

            while (true) {
                std::cout << "fetching statement result" << std::endl;
                status = mysql_stmt_fetch(stmt);

                if (status == 1 || status == MYSQL_NO_DATA) {
                    break;
                }

                alb.title = title;
                albums.push_back(std::move(alb));
            }
        }
        std::cout << "fetching next result" << std::endl;
    }

    return albums;
}

// TODO: check to see if this is not used, if not then remove it
model::Album AlbumRepository::parseRecord(MYSQL_RES* results)
{
    std::cout << "parsing album record" << std::endl;
    model::Album album;

    auto fieldNum = mysql_num_fields(results);
    auto row = mysql_fetch_row(results);

    for (auto i = 0; i != fieldNum; ++i) {
        const std::string field(mysql_fetch_field(results)->name);

        if (field.compare("AlbumId") == 0) {
            album.id = std::stoi(row[i]);
        }
        if (field.compare("Title") == 0) {
            album.title = row[i];
        }
        if (field.compare("Year") == 0) {
            album.year = std::stoi(row[i]);
        }
    }

    return album;
}

std::pair<model::Album, int> AlbumRepository::parseRecordWithSongCount(MYSQL_STMT *stmt)
{
    std::cout << "parsing album record with song count" << std::endl;
    mysql_stmt_store_result(stmt);
    auto rowCount = mysql_stmt_num_rows(stmt);

    model::Album album;
    int songCount = 0;

    if (rowCount == 0) {
        std::cout << "no results" << std::endl;
        return std::make_pair(album, songCount);
    }

    if (mysql_stmt_field_count(stmt) == 0) {
        std::cout << "field count is 0, must be an incorrect query" << std::endl;
        return std::make_pair(model::Album(), 0);
    }

    auto res = mysql_stmt_result_metadata(stmt);
    constexpr auto valAmt = 4;
    constexpr auto strLen = 1024;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    MYSQL_BIND val[valAmt];
    std::memset(val, 0, sizeof(val));

    char title[strLen];

    val[0].buffer_type = MYSQL_TYPE_LONG;
    val[0].buffer = (char*)&album.id;
    val[0].length = &len[0];
    val[0].is_null = &nullRes[0];

    val[1].buffer_type = MYSQL_TYPE_STRING;
    val[1].buffer = (char*)title;
    val[1].buffer_length = strLen;
    val[1].length = &len[1];
    val[1].is_null = &nullRes[1];

    val[2].buffer_type = MYSQL_TYPE_LONG;
    val[2].buffer = (char*)&album.year;
    val[2].length = &len[2];
    val[2].is_null = &nullRes[2];

    val[3].buffer_type = MYSQL_TYPE_LONG;
    val[3].buffer = (char*)&songCount;
    val[3].length = &len[3];
    val[3].is_null = &nullRes[3];

    auto status = mysql_stmt_bind_result(stmt, val);
    status = mysql_stmt_fetch(stmt);

    album.title = std::move(title);

    std::cout << "done parsing album record with song count" << std::endl;

    auto albWSC = std::make_pair(album, songCount);

    return albWSC;
}

model::Album AlbumRepository::parseRecord(MYSQL_STMT *stmt)
{
    std::cout << "parsing album record" << std::endl;
    mysql_stmt_store_result(stmt);
    auto rows = mysql_stmt_num_rows(stmt);

    model::Album album;
    auto status = 0;
    auto time = 0;
    auto valAmt = 3;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    while (status == 0) {
        if (mysql_stmt_field_count(stmt) > 0) {
            auto res = mysql_stmt_result_metadata(stmt);
            auto fields = mysql_fetch_fields(res);
            auto strLen = 1024;

            MYSQL_BIND val[valAmt];
            memset(val, 0, sizeof(val));

            char title[strLen];

            val[0].buffer_type = MYSQL_TYPE_LONG;
            val[0].buffer = (char*)&album.id;
            val[0].length = &len[0];
            val[0].is_null = &nullRes[0];

            val[1].buffer_type = MYSQL_TYPE_STRING;
            val[1].buffer = (char*)title;
            val[1].buffer_length = strLen;
            val[1].length = &len[1];
            val[1].is_null = &nullRes[1];

            val[2].buffer_type = MYSQL_TYPE_LONG;
            val[2].buffer = (char*)&album.year;
            val[2].length = &len[2];
            val[2].is_null = &nullRes[2];

            status = mysql_stmt_bind_result(stmt, val);
            mysql_stmt_store_result(stmt);

            status = mysql_stmt_fetch(stmt);

            album.title = title;
        }

        status = mysql_stmt_next_result(stmt);
    }

    std::cout << "done parsing album record" << std::endl;

    return album;
}
}
