#include "database/artistRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

Database::artistRepository::artistRepository(const Model::BinaryPath& binConf)
    : base_repository(binConf)
{
}


Model::Artist Database::artistRepository::retrieveRecord(Model::Artist& artist, Type::artistFilter filter)
{
    std::cout << "retrieving artist record" << std::endl;
    std::stringstream qry;
    auto conn = setup_mysql_connection();
    qry << "SELECT art.* FROM Artist art WHERE ";

    std::unique_ptr<char*> param;
    switch (filter) {
        case Type::artistFilter::id:
            qry << "art.ArtistId = " << artist.id;
            break;
        case Type::artistFilter::artist:
            param = std::make_unique<char*>(new char[artist.artist.size()]);
            mysql_real_escape_string(conn, *param, artist.artist.c_str(), artist.artist.size());
            qry << "art.Artist ='" << *param << "'";
            break;
        default:
            break;
    }

    qry << " ORDER BY ArtistId DESC LIMIT 1";

    const auto query = qry.str();
    auto results = perform_mysql_query(conn, query);

    artist = parseRecord(results);

    mysql_close(conn);

    std::cout << "retrieved record" << std::endl;

    return artist;
}

void Database::artistRepository::saveRecord(const Model::Artist& artist)
{
    std::cout << "inserting artist record" << std::endl;

    auto conn = setup_mysql_connection();
    MYSQL_STMT *stmt = mysql_stmt_init(conn);

    const std::string query = "INSERT INTO Artist(Artist) VALUES(?)";

    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)artist.artist.c_str();
    auto artistLength = artist.artist.size();
    params[0].length = &artistLength;
    params[0].is_null = 0;

    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    std::cout<< "inserted artist record" << std::endl;
}


Model::Artist Database::artistRepository::parseRecord(MYSQL_RES* results)
{
    std::cout << "parsing artist record" << std::endl;
    Model::Artist artist;

    auto fieldNum = mysql_num_fields(results);
    auto row = mysql_fetch_row(results);

    for (auto i = 0; i != fieldNum; ++i) {
        const std::string field(mysql_fetch_field(results)->name);

        if (field.compare("ArtistId") == 0) {
            artist.id = std::stoi(row[i]);
        }
        if (field.compare("Artist") == 0) {
            artist.artist = row[i];
        }
    }

    std::cout << "parsed artist record" << std::endl;

    return artist;
}
