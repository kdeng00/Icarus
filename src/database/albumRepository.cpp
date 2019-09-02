#include "database/albumRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

Database::albumRepository::albumRepository(const Model::BinaryPath& bConf)
    : base_repository(bConf)
{ }


// TODO: implement this later on
std::vector<Model::Album> Database::albumRepository::retrieveRecords()
{
    std::vector<Model::Album> albums;

    return albums;
}

Model::Album Database::albumRepository::retrieveRecord(Model::Album& album, Type::albumFilter filter)
{
    std::cout << "retrieving album record" << std::endl;
    std::stringstream qry;
    auto conn = setup_mysql_connection();
    qry << "SELECT alb.* FROM Album alb WHERE ";

    std::unique_ptr<char*> param;
    switch (filter) {
        case Type::albumFilter::id:
            qry << "alb.AlbumId = " << album.id;
            break;
        case Type::albumFilter::title:
            param = std::make_unique<char*>(new char[album.title.size()]);
            mysql_real_escape_string(conn, *param, album.title.c_str(), album.title.size());
            qry << "alb.Title = '" << *param << "'";
            break;
        default:
            break;
    }

    qry << " ORDER BY AlbumId DESC LIMIT 1";

    const std::string query = qry.str();
    auto results = perform_mysql_query(conn, query);

    album = parseRecord(results);

    mysql_close(conn);
    std::cout << "done" << std::endl;

    return album;
}

void Database::albumRepository::saveAlbum(const Model::Album& album)
{
    std::cout << "beginning to insert album record" << std::endl;

    auto conn = setup_mysql_connection();
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
std::vector<Model::Album> Database::albumRepository::parseRecords(MYSQL_RES* results)
{
    std::vector<Model::Album> albums;

    return albums;
}

Model::Album Database::albumRepository::parseRecord(MYSQL_RES* results)
{
    std::cout << "parsing album record" << std::endl;
    Model::Album album;

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
