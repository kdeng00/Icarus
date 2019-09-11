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


std::vector<model::Artist> database::ArtistRepository::retrieveRecords()
{
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

std::pair<model::Artist, int> database::ArtistRepository::retrieveRecordWithSongCount(model::Artist& artist, type::ArtistFilter filter = type::ArtistFilter::id)
{
    std::cout << "retrieving artist record with song count" << std::endl;
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
    std::cout << "has been performed" << std::endl;

    auto artWSC = parseRecordWithSongCount(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return artWSC;
}

model::Artist database::ArtistRepository::retrieveRecord(model::Artist& artist, type::ArtistFilter filter)
{
    std::cout << "retrieving artist record" << std::endl;
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);
    qry << "SELECT art.* FROM Artist art WHERE ";

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    auto artistLength = artist.artist.size();
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
            params[0].buffer = (char*)artist.artist.c_str();
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

    std::cout << "retrieved record" << std::endl;

    return artist;
}

bool database::ArtistRepository::doesArtistExist(const model::Artist& artist, type::ArtistFilter filter)
{
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    std::stringstream qry;
    qry << "SELECT * FROM Artist WHERE ";

    auto artistLength = artist.artist.size();
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
            params[0].buffer = (char*)artist.artist.c_str();
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

    std::cout << "the query has been performed" << std::endl;

    mysql_stmt_store_result(stmt);
    auto rowCount = mysql_stmt_num_rows(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);
    
    return (rowCount > 0) ? true : false;
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

void database::ArtistRepository::deleteArtist(const model::Artist& artist, type::ArtistFilter filter = type::ArtistFilter::id) {
    // TODO: implement this
    std::cout << "delete Artist record" << std::endl;
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

    std::cout << "deleted artist record" << std::endl;
}

std::vector<model::Artist> database::ArtistRepository::parseRecords(MYSQL_STMT *stmt)
{
    mysql_stmt_store_result(stmt);

    std::vector<model::Artist> artists;
    artists.reserve(mysql_stmt_num_rows(stmt));

    if (mysql_stmt_field_count(stmt) == 0) {
        std::cout << "field count is 0" << std::endl;
        return artists;
    }

    model::Artist art;
    const auto valAmt = 2;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    auto res = mysql_stmt_result_metadata(stmt);
    auto fields = mysql_fetch_fields(res);
    const auto strLen = 1024;

    char artist[strLen];

    MYSQL_BIND val[valAmt];
    memset(val, 0, sizeof(val));

    val[0].buffer_type = MYSQL_TYPE_LONG;
    val[0].buffer = (char*)&art.id;
    val[0].length = &len[0];
    val[0].is_null = &nullRes[0];

    val[1].buffer_type = MYSQL_TYPE_STRING;
    val[1].buffer = (char*)artist;
    val[1].buffer_length = strLen;
    val[1].length = &len[1];
    val[1].is_null = &nullRes[1];

    for (auto status = mysql_stmt_bind_result(stmt, val); status == 0; ) {
        std::cout << "fetching statement result" << std::endl;
        status = mysql_stmt_fetch(stmt);

        if (status == 0) {
            art.artist = artist;
            artists.push_back(std::move(art));
        }
    }

    return artists;
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

std::pair<model::Artist, int> database::ArtistRepository::parseRecordWithSongCount(MYSQL_STMT *stmt)
{
    std::cout << "parsing artist record with song count" << std::endl;
    mysql_stmt_store_result(stmt);

    const auto strLen = 1024;
    const auto valAmt = 3;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    model::Artist artist;
    int songCount = 0;

    if (mysql_stmt_num_rows(stmt) == 0) {
        std::cout << "no results" << std::endl;
        return std::make_pair(artist, songCount);
    }

    MYSQL_BIND params[valAmt];
    std::memset(params, 0, sizeof(params));

    char art[strLen];

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (char*)&artist.id;
    params[0].length = &len[0];
    params[0].is_null = &nullRes[0];

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)art;
    params[1].buffer_length = strLen;
    params[1].length = &len[1];
    params[1].is_null = &nullRes[1];
    
    params[2].buffer_type = MYSQL_TYPE_LONG;
    params[2].buffer = (char*)&songCount;
    params[2].length = &len[2];
    params[2].is_null = &nullRes[2];

    mysql_stmt_bind_result(stmt, params);
    mysql_stmt_fetch(stmt);

    artist.artist = std::move(art);

    std::cout << "parsed artist record with song count" << std::endl;

    return std::make_pair(artist, songCount);
}

model::Artist database::ArtistRepository::parseRecord(MYSQL_STMT *stmt)
{
    std::cout << "parsing artist record" << std::endl;
    mysql_stmt_store_result(stmt);

    model::Artist art;
    auto status = 0;
    auto time = 0;
    auto valAmt = 2;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    while (status == 0) {
        if (mysql_stmt_field_count(stmt) > 0) {
            auto res = mysql_stmt_result_metadata(stmt);
            auto fields = mysql_fetch_fields(res);
            auto strLen = 1024;

            MYSQL_BIND val[valAmt];
            memset(val, 0, sizeof(val));

            char artist[strLen];

            val[0].buffer_type = MYSQL_TYPE_LONG;
            val[0].buffer = (char*)&art.id;
            val[0].length = &len[0];
            val[0].is_null = &nullRes[0];

            val[1].buffer_type = MYSQL_TYPE_STRING;
            val[1].buffer = (char*)&artist;
            val[1].buffer_length = strLen;
            val[1].length = &len[1];
            val[1].is_null = &nullRes[1];

            status = mysql_stmt_bind_result(stmt, val);
            mysql_stmt_store_result(stmt);

            status = mysql_stmt_fetch(stmt);

            art.artist = artist;
        }

        status = mysql_stmt_next_result(stmt);
    }

    std::cout << "done parsing artist record" << std::endl;

    return art;
}
