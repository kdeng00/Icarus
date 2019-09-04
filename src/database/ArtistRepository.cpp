#include "database/ArtistRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

database::ArtistRepository::ArtistRepository(const model::BinaryPath& binConf)
    : BaseRepository(binConf)
{
}


model::Artist database::ArtistRepository::retrieveRecord(model::Artist& artist, type::ArtistFilter filter)
{
    std::cout << "retrieving artist record" << std::endl;
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    qry << "SELECT art.* FROM Artist art WHERE ";

    std::unique_ptr<char*> param;
    switch (filter) {
        case type::ArtistFilter::id:
            qry << "art.ArtistId = " << artist.id;
            break;
        case type::ArtistFilter::artist:
            param = std::make_unique<char*>(new char[artist.artist.size()]);
            mysql_real_escape_string(conn, *param, artist.artist.c_str(), artist.artist.size());
            qry << "art.Artist ='" << *param << "'";
            break;
        default:
            break;
    }

    qry << " ORDER BY ArtistId DESC LIMIT 1";

    const auto query = qry.str();
    auto results = performMysqlQuery(conn, query);

    artist = parseRecord(results);

    mysql_close(conn);

    std::cout << "retrieved record" << std::endl;

    return artist;
}

bool database::ArtistRepository::doesArtistExist(const model::Artist& artist, type::ArtistFilter filter)
{
    // TODO: implement this
    
    return false;
}

void database::ArtistRepository::saveRecord(const model::Artist& artist)
{
    std::cout << "inserting artist record" << std::endl;

    auto conn = setupMysqlConnection();
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


model::Artist database::ArtistRepository::parseRecord(MYSQL_RES* results)
{
    std::cout << "parsing artist record" << std::endl;
    model::Artist artist;

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

model::Artist database::ArtistRepository::parseRecord(MYSQL_STMT *stmt)
{
    // TODO: implement this
    model::Artist art;

    return art;
}
