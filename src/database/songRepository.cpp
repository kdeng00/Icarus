#include "database/songRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

songRepository::songRepository(const std::string& path) : base_repository(path)
{ }

Song songRepository::retrieveRecord(Song& song, songFilter)
{

    return song;
}

void songRepository::saveRecord(const Song& song)
{
    // TODO: Yeeesh! Clean this shit up!
    auto conn = setup_mysql_connection();
    auto status = 0;

    MYSQL_STMT *stmt;
    stmt = mysql_stmt_init(conn);

    std::string query  = "INSERT INTO Song(Title, Artist, Album, Genre, ";
    query.append("Year, Duration, CoverArtId) VALUES(?, ?, ?, ?, ?, ?, ?)");

    status = mysql_stmt_prepare(stmt, query.c_str(), query.size());


    //auto params = bindParams(song);
    //MYSQL_BIND *b;
    //b = *params.get();
    //auto status = mysql_stmt_bind_param(stmt, b);
    //
    MYSQL_BIND params[7];

    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)song.title.c_str();
    auto titleLength = song.title.size();
    params[0].length = &titleLength;
    params[0].is_null = 0;

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)song.artist.c_str();
    auto artistLength = song.artist.size();
    params[1].length = &artistLength;
    params[1].is_null = 0;

    params[2].buffer_type = MYSQL_TYPE_STRING;
    params[2].buffer = (char*)song.album.c_str();
    auto albumLength = song.album.size();
    params[2].length = &albumLength;
    params[2].is_null = 0;

    params[3].buffer_type = MYSQL_TYPE_STRING;
    params[3].buffer = (char*)song.genre.c_str();
    auto genreLength = song.genre.size();
    params[3].length = &genreLength;
    params[3].is_null = 0;

    params[4].buffer_type = MYSQL_TYPE_LONG;
    params[4].buffer = (char*)&song.year;
    params[4].length = 0;
    params[4].is_null = 0;

    params[5].buffer_type = MYSQL_TYPE_LONG;
    params[5].buffer = (char*)&song.duration;
    params[5].length = 0;
    params[5].is_null = 0;

    params[6].buffer_type = MYSQL_TYPE_LONG;
    params[6].buffer = (char*)&song.coverArtId;
    params[6].length = 0;
    params[6].is_null = 0;
    
    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    std::cout << "done inserting song record" << std::endl;
}

std::vector<Song> songRepository::parseRecords(MYSQL_RES* results)
{
    std::vector<Song> songs;


    return songs;
}

std::unique_ptr<MYSQL_BIND*> songRepository::bindParams(const Song& song)
//std::unique_ptr<MYSQL_BIND[]> songRepository::bindParams(const Song& song)
//std::shared_ptr<MYSQL_BIND> songRepository::bindParams(const Song& song)
{
    MYSQL_BIND params[7];

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)song.title.c_str();
    auto titleLength = song.title.size();
    params[0].length = &titleLength;
    params[0].is_null = 0;

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)song.artist.c_str();
    auto artistLength = song.artist.size();
    params[1].length = &artistLength;
    params[1].is_null = 0;

    params[2].buffer_type = MYSQL_TYPE_STRING;
    params[2].buffer = (char*)song.album.c_str();
    auto albumLength = song.album.size();
    params[2].length = &albumLength;
    params[2].is_null = 0;

    params[3].buffer_type = MYSQL_TYPE_STRING;
    params[3].buffer = (char*)song.genre.c_str();
    auto genreLength = song.genre.size();
    params[3].length = &genreLength;
    params[3].is_null = 0;

    params[4].buffer_type = MYSQL_TYPE_LONG;
    params[4].buffer = (char*)&song.year;
    params[4].length = 0;
    params[4].is_null = 0;

    params[5].buffer_type = MYSQL_TYPE_LONG;
    params[5].buffer = (char*)&song.duration;
    params[5].length = 0;
    params[5].is_null = 0;

    params[6].buffer_type = MYSQL_TYPE_LONG;
    params[6].buffer = (char*)&song.coverArtId;
    params[6].length = 0;
    params[6].is_null = 0;

    std::cout << "binding params" << std::endl;

    //return std::make_unique<MYSQL_BIND*>(params);
    return std::make_unique<MYSQL_BIND*>(params);
    //return std::make_shared<MYSQL_BIND>(params);
}

Song songRepository::parseRecord(MYSQL_RES* results)
{
    Song song;

    return song;
}
