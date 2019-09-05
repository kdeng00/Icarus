#include "database/AlbumRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

database::AlbumRepository::AlbumRepository(const model::BinaryPath& bConf)
    : BaseRepository(bConf)
{ }


// TODO: implement this later on
std::vector<model::Album> database::AlbumRepository::retrieveRecords()
{
    std::vector<model::Album> albums;

    return albums;
}

model::Album database::AlbumRepository::retrieveRecord(model::Album& album, type::AlbumFilter filter)
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
            qry << "alb.AlbumId = " << album.id;

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

bool database::AlbumRepository::doesAlbumExists(const model::Album& album, type::AlbumFilter filter)
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

void database::AlbumRepository::saveAlbum(const model::Album& album)
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


// TODO: implement this
std::vector<model::Album> database::AlbumRepository::parseRecords(MYSQL_RES* results)
{
    std::vector<model::Album> albums;

    return albums;
}

model::Album database::AlbumRepository::parseRecord(MYSQL_RES* results)
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

model::Album database::AlbumRepository::parseRecord(MYSQL_STMT *stmt)
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
