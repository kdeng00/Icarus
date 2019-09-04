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
    qry << "SELECT alb.* FROM Album alb WHERE ";

    std::unique_ptr<char*> param;
    switch (filter) {
        case type::AlbumFilter::id:
            qry << "alb.AlbumId = " << album.id;
            break;
        case type::AlbumFilter::title:
            param = std::make_unique<char*>(new char[album.title.size()]);
            mysql_real_escape_string(conn, *param, album.title.c_str(), album.title.size());
            qry << "alb.Title = '" << *param << "'";
            break;
        default:
            break;
    }

    qry << " ORDER BY AlbumId DESC LIMIT 1";

    const std::string query = qry.str();
    auto results = performMysqlQuery(conn, query);

    album = parseRecord(results);

    mysql_close(conn);
    std::cout << "done" << std::endl;

    return album;
}

bool database::AlbumRepository::doesAlbumExists(const model::Album& album, type::AlbumFilter filter)
{
    // TODO: continue working on this part.
    // Reason: there should be a check to see if an album record already exists
    // at the moment there is no check and every time a song is uploaded
    // a new album record is created, even for songs that are on the
    // same album. This should not happen. There should be one album
    // record for songs on the same album. After fixing this, do the
    // same for Artist, Genre, and Year records

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
    // TODO: implement this
    model::Album album;

    return album;
}
