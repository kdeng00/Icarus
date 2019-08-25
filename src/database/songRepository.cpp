#include "database/songRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

#include "types/songFilter.h"

songRepository::songRepository(const std::string& path) : base_repository(path)
{ }

std::vector<Song> songRepository::retrieveRecords()
{
    auto conn = setup_mysql_connection();
    const std::string query = "SELECT * FROM Song";

    auto results = perform_mysql_query(conn, query);
    auto songs = parseRecords(results);

    mysql_close(conn);


    return songs;
}

Song songRepository::retrieveRecord(Song& song, songFilter filter)
{
    std::stringstream qry;
    auto conn = setup_mysql_connection();
    qry << "SELECT * FROM Song WHERE ";

    std::unique_ptr<char*> param;
    switch (filter) {
        case songFilter::id:
            qry << "SongId = " << song.id;
            break;
        case songFilter::title:
            param = std::make_unique<char*>(new char[song.title.size()]);
            mysql_real_escape_string(conn, *param, song.title.c_str(), song.title.size());
            std::cout << *param << std::endl;
            qry << "Title = '" << *param << "'";
            break;
        default:
            break;
    }

    const std::string query = qry.str();
    auto results = perform_mysql_query(conn, query);
    std::cout << "the query has been performed" << std::endl;

    song = parseRecord(results);

    mysql_close(conn);
    std::cout << "done" << std::endl;

    return song;
}

void songRepository::saveRecord(const Song& song)
{
    auto conn = setup_mysql_connection();
    auto status = 0;

    MYSQL_STMT *stmt = mysql_stmt_init(conn);

    std::string query  = "INSERT INTO Song(Title, Artist, Album, Genre, ";
    query.append("Year, Duration, CoverArtId) VALUES(?, ?, ?, ?, ?, ?, ?)");

    status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

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
    auto fieldNum = mysql_num_fields(results);
    auto numRows = mysql_num_rows(results);

    std::vector<Song> songs;
    songs.reserve(numRows);

    for (MYSQL_ROW row = nullptr; (row = mysql_fetch_row(results)) != nullptr; ) {
        Song song;

        for (auto i = 0; i != fieldNum; ++i) {
            switch (i) {
                case 0:
                    song.id = std::stoi(row[i]);
                    break;
                case 1:
                    song.title = row[i];
                    break;
                case 2:
                    song.artist= row[i];
                    break;
                case 3:
                    song.album = row[i];
                    break;
                case 4:
                    song.genre = row[i];
                    break;
                case 5:
                    song.year = std::stoi(row[i]);
                    break;
                case 6:
                    song.duration = std::stoi(row[i]);
                    break;
                case 7:
                    song.coverArtId = std::stoi(row[i]);
                    break;
            }
        }

        songs.push_back(song);
    }

    return songs;
}

Song songRepository::parseRecord(MYSQL_RES* results)
{
    Song song;
    auto fieldNum = mysql_num_fields(results);

    MYSQL_ROW row = mysql_fetch_row(results);

    for (auto i = 0; i != fieldNum; ++i) {
        switch (i) {
            case 0:
                song.id = std::stoi(row[i]);
                break;
            case 1:
                song.title = row[i];
                break;
            case 2:
                song.artist= row[i];
                break;
            case 3:
                song.album = row[i];
                break;
            case 4:
                song.genre = row[i];
                break;
            case 5:
                song.year = std::stoi(row[i]);
                break;
            case 6:
                song.duration = std::stoi(row[i]);
                break;
            case 7:
                song.coverArtId = std::stoi(row[i]);
        }
    }
    std::cout << "done parsing record" << std::endl;

    return song;
}
