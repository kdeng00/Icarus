#include "database/SongRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

#include "type/SongFilter.h"

database::SongRepository::SongRepository(const std::string& path) : BaseRepository(path)
{ }

database::SongRepository::SongRepository(const model::BinaryPath& bConf) : BaseRepository(bConf)
{ }

std::vector<model::Song> database::SongRepository::retrieveRecords()
{
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);
    const std::string query = "SELECT * FROM Song";

    ::mysql_stmt_prepare(stmt, query.c_str(), query.size());
    ::mysql_stmt_execute(stmt);

    auto songs = parseRecords(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return songs;
}

model::Song database::SongRepository::retrieveRecord(model::Song& song, type::SongFilter filter)
{
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    qry << "SELECT * FROM Song WHERE ";

    auto titleLength = song.title.size();
    switch (filter) {
        case type::SongFilter::id:
            qry << "SongId = ?";

            params[0].buffer_type = MYSQL_TYPE_LONG;
            params[0].buffer = (char*)&song.id;
            params[0].length = 0;
            params[0].is_null = 0;;
            break;
        case type::SongFilter::title:
            qry << "Title = ?";

            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char*)song.title.c_str();
            params[0].length = &titleLength;
            params[0].is_null = 0;
            break;
        default:
            break;
    }

    qry << " LIMIT 1";

    const std::string query = qry.str();
    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    std::cout << "the query has been performed" << std::endl;

    song = parseRecord(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);
    std::cout << "done" << std::endl;

    return song;
}

bool database::SongRepository::doesSongExist(const model::Song& song, type::SongFilter filter)
{
    std::cout << "checking to see if song exists" << std::endl;
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    qry << "SELECT * FROM Song WHERE ";

    auto titleLength = song.title.size();
    switch (filter) {
        case type::SongFilter::id:
            qry << "SongId = ?";

            params[0].buffer_type = MYSQL_TYPE_LONG;
            params[0].buffer = (char*)&song.id;
            params[0].length = 0;
            params[0].is_null = 0;
            break;
        case type::SongFilter::title:
            qry << "Title = ?";

            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char*)song.title.c_str();
            params[0].length = &titleLength;
            params[0].is_null = 0;
            break;
        default:
            break;
    }

    qry << " LIMIT 1";

    const std::string query = qry.str();
    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    std::cout << "the query has been performed" << std::endl;

    ::mysql_stmt_store_result(stmt);
    auto rowCount = ::mysql_stmt_num_rows(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);
    std::cout << "done" << std::endl;

    return (rowCount > 0) ? true : false;
}

void database::SongRepository::deleteRecord(const model::Song& song)
{
    auto conn = setupMysqlConnection();
    auto status = 0;

    const std::string query("DELETE FROM Song WHERE SongId = " + std::to_string(song.id));

    auto result = performMysqlQuery(conn, query);

    mysql_close(conn);
}

void database::SongRepository::saveRecord(const model::Song& song)
{
    std::cout << "beginning to insert song record" << std::endl;
    auto conn = setupMysqlConnection();
    auto status = 0;

    MYSQL_STMT *stmt = mysql_stmt_init(conn);

    std::string query = "INSERT INTO Song(Title, Artist, Album, Genre, ";
    query.append("Year, Duration, Track, Disc, SongPath, CoverArtId, ArtistId, ");
    query.append("AlbumId, GenreId, YearId) VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

    MYSQL_BIND params[14];
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
    params[6].buffer = (char*)&song.track;
    params[6].length = 0;
    params[6].is_null = 0;

    params[7].buffer_type = MYSQL_TYPE_LONG;
    params[7].buffer = (char*)&song.disc;
    params[7].length = 0;
    params[7].is_null = 0;

    params[8].buffer_type = MYSQL_TYPE_STRING;
    params[8].buffer = (char*)song.songPath.c_str();
    auto pathLength = song.songPath.size();
    params[8].length = &pathLength;
    params[8].is_null = 0;

    params[9].buffer_type = MYSQL_TYPE_LONG;
    params[9].buffer = (char*)&song.coverArtId;
    params[9].length = 0;
    params[9].is_null = 0;

    params[10].buffer_type = MYSQL_TYPE_LONG;
    params[10].buffer = (char*)&song.artistId;
    params[10].length = 0;
    params[10].is_null = 0;

    params[11].buffer_type = MYSQL_TYPE_LONG;
    params[11].buffer = (char*)&song.albumId;
    params[11].length = 0;
    params[11].is_null = 0;
    
    params[12].buffer_type = MYSQL_TYPE_LONG;
    params[12].buffer = (char*)&song.genreId;
    params[12].length = 0;
    params[12].is_null = 0;

    params[13].buffer_type = MYSQL_TYPE_LONG;
    params[13].buffer = (char*)&song.yearId;
    params[13].length = 0;
    params[13].is_null = 0;

    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    std::cout << "done inserting song record" << std::endl;
}

// TODO: delete this, not being used
std::vector<model::Song> database::SongRepository::parseRecords(MYSQL_RES* results)
{
    auto fieldNum = mysql_num_fields(results);
    auto numRows = mysql_num_rows(results);

    std::vector<model::Song> songs;
    songs.reserve(numRows);

    for (MYSQL_ROW row = nullptr; (row = mysql_fetch_row(results)) != nullptr; ) {
        model::Song song;

        for (auto i = 0; i != fieldNum; ++i) {
            /**
            auto field = mysql_fetch_field(results);
            if (field->name  == NULL) {
                std::cout << "null field" << std::endl;
                break;
            }
            */
            /**
            std::string field;
            if ((i + 0) == fieldNum) {
                std::cout << "goodbye" << std::endl;
                break;
            } else {
                std::cout << "still good" << std::endl;
                field.assign(mysql_fetch_field(results)->name);
            }
            //const std::string field(row[i].field);
            std::cout << field << std::endl;

            if (field.compare("SongId") == 0) {
                song.id = std::stoi(row[i]);
            }
            if (field.compare("Title") == 0) {
                song.title = row[i];
            }
            if (field.compare("Artist") == 0) {
                song.artist = row[i];
            }
            if (field.compare("Album") == 0) {
                song.album = row[i];
            }
            if (field.compare("Genre") == 0) {
                song.genre = row[i];
            }
            if (field.compare("Year") == 0) {
                song.year = std::stoi(row[i]);
            }
            if (field.compare("Duration") == 0) {
                song.duration = std::stoi(row[i]);
            }
            if (field.compare("Track") == 0) {
                song.track = std::stoi(row[i]);
            }
            if (field.compare("Disc") == 0) {
                song.disc = std::stoi(row[i]);
            }
            if (field.compare("SongPath") == 0) {
                song.songPath = row[i];
            }
            if (field.compare("CoverArtId") == 0) {
                song.coverArtId = std::stoi(row[i]);
            }
            */
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
                    song.track = std::stoi(row[i]);
                    break;
                case 8:
                    song.disc = std::stoi(row[i]);
                    break;
                case 9:
                    song.songPath = row[i];
                    break;
                case 10:
                    song.coverArtId = std::stoi(row[i]);
                    break;
            }
        }

        songs.push_back(song);
    }

    return songs;
}

std::vector<model::Song> database::SongRepository::parseRecords(MYSQL_STMT *stmt)
{
    ::mysql_stmt_store_result(stmt);
    auto c = ::mysql_stmt_num_rows(stmt);
    std::cout << "number of results " << c << std::endl;
    std::vector<model::Song> songs;
    songs.reserve(c);

    auto status = 0;
    auto time = 0;
    auto valAmt = 15;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    while (status == 0) {
        std::cout << time++ << " time" << std::endl;
        std::cout << "field count " << ::mysql_stmt_field_count(stmt) << std::endl;
        
        if (::mysql_stmt_field_count(stmt) > 0) {

            model::Song song;
            auto res = ::mysql_stmt_result_metadata(stmt);
            auto fields = ::mysql_fetch_fields(res);
            auto strLen = 1024;
            MYSQL_BIND val[valAmt];
            memset(val, 0, sizeof(val));

            char title[strLen];
            char album[strLen];
            char artist[strLen];
            char genre[strLen];
            char path[strLen];

            val[0].buffer_type = MYSQL_TYPE_LONG;
            val[0].buffer = (char*)&song.id;
            val[0].length = &len[0];
            val[0].is_null = &nullRes[0];

            val[1].buffer_type = MYSQL_TYPE_STRING;
            val[1].buffer = (char*)title;
            val[1].buffer_length = strLen;
            val[1].length = &len[1];
            val[1].is_null = &nullRes[1];

            val[2].buffer_type = MYSQL_TYPE_STRING;
            val[2].buffer = (char*)artist;
            val[2].buffer_length = strLen;
            val[2].length = &len[2];
            val[2].is_null = &nullRes[2];

            val[3].buffer_type = MYSQL_TYPE_STRING;
            val[3].buffer = (char*)album;
            val[3].buffer_length = strLen;
            val[3].length = &len[3];
            val[3].is_null = &nullRes[3];

            val[4].buffer_type = MYSQL_TYPE_STRING;
            val[4].buffer = (char*)genre;
            val[4].buffer_length = strLen;
            val[4].length = &len[4];
            val[4].is_null = &nullRes[4];

            val[5].buffer_type = MYSQL_TYPE_LONG;
            val[5].buffer = (char*)&song.year;
            val[5].length = &len[5];
            val[5].is_null = &nullRes[5];

            val[6].buffer_type = MYSQL_TYPE_LONG;
            val[6].buffer = (char*)&song.duration;
            val[6].length = &len[6];
            val[6].is_null = &nullRes[6];

            val[7].buffer_type = MYSQL_TYPE_LONG;
            val[7].buffer = (char*)&song.track;
            val[7].length = &len[7];
            val[7].is_null = &nullRes[7];

            val[8].buffer_type = MYSQL_TYPE_LONG;
            val[8].buffer = (char*)&song.disc;
            val[8].length = &len[8];
            val[8].is_null = &nullRes[8];

            val[9].buffer_type = MYSQL_TYPE_STRING;
            val[9].buffer = (char*)path;
            val[9].buffer_length = strLen;
            val[9].length = &len[9];
            val[9].is_null = &nullRes[9];

            val[10].buffer_type = MYSQL_TYPE_LONG;
            val[10].buffer = (char*)&song.coverArtId;
            val[10].length = &len[10];
            val[10].is_null = &nullRes[10];

            val[11].buffer_type = MYSQL_TYPE_LONG;
            val[11].buffer = (char*)&song.artistId;
            val[11].length = &len[11];
            val[11].is_null = &nullRes[11];

            val[12].buffer_type = MYSQL_TYPE_LONG;
            val[12].buffer = (char*)&song.albumId;;
            val[12].length = &len[12];
            val[12].is_null = &nullRes[12];

            val[13].buffer_type = MYSQL_TYPE_LONG;
            val[13].buffer = (char*)&song.genreId;
            val[13].length = &len[13];
            val[13].is_null = &nullRes[13];

            val[14].buffer_type = MYSQL_TYPE_LONG;
            val[14].buffer = (char*)&song.yearId;
            val[14].length = &len[14];
            val[14].is_null = &nullRes[14];

            status = ::mysql_stmt_bind_result(stmt, val);

            while (1) {
                std::cout << "fetching statement result" << std::endl;
                status = ::mysql_stmt_fetch(stmt);

                if (status == 1 || status == MYSQL_NO_DATA) {
                    break;
                }
                song.title = title;
                song.album = album;
                song.artist = artist;
                song.genre = genre;
                song.songPath = path;

                songs.push_back(song);
            }
        }
        std::cout << "fetching next result" << std::endl;
        status = ::mysql_stmt_next_result(stmt);
    }

    return songs;
}


// TODO: delete this, not being used anymore
model::Song database::SongRepository::parseRecord(MYSQL_RES* results)
{
    model::Song song;

    auto fieldNum = mysql_num_fields(results);
    auto row = mysql_fetch_row(results);

    for (auto i = 0; i != fieldNum; ++i) {
        const std::string field(mysql_fetch_field(results)->name);

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
                song.track = std::stoi(row[i]);
                break;
            case 8:
                song.disc = std::stoi(row[i]);
                break;
            case 9:
                song.songPath = row[i];
                break;
            case 10:
                song.coverArtId = std::stoi(row[i]);
                break;
        }
    }
    std::cout << "done parsing record" << std::endl;

    return song;
}

model::Song database::SongRepository::parseRecord(MYSQL_STMT *stmt)
{
    model::Song song;
    auto status = 0;
    auto time = 0;
    auto valAmt = 15;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    while (status == 0) {
        if (::mysql_stmt_field_count(stmt) > 0) {
            auto res = ::mysql_stmt_result_metadata(stmt);
            auto fields = ::mysql_fetch_fields(res);
            auto strLen = 1024;
            MYSQL_BIND val[valAmt];
            memset(val, 0, sizeof(val));

            char title[strLen];
            char album[strLen];
            char artist[strLen];
            char genre[strLen];
            char path[strLen];

            val[0].buffer_type = MYSQL_TYPE_LONG;
            val[0].buffer = (char*)&song.id;
            val[0].length = &len[0];
            val[0].is_null = &nullRes[0];

            val[1].buffer_type = MYSQL_TYPE_STRING;
            val[1].buffer = (char*)title;
            val[1].buffer_length = strLen;
            val[1].length = &len[1];
            val[1].is_null = &nullRes[1];

            val[2].buffer_type = MYSQL_TYPE_STRING;
            val[2].buffer = (char*)artist;
            val[2].buffer_length = strLen;
            val[2].length = &len[2];
            val[2].is_null = &nullRes[2];

            val[3].buffer_type = MYSQL_TYPE_STRING;
            val[3].buffer = (char*)album;
            val[3].buffer_length = strLen;
            val[3].length = &len[3];
            val[3].is_null = &nullRes[3];

            val[4].buffer_type = MYSQL_TYPE_STRING;
            val[4].buffer = (char*)genre;
            val[4].buffer_length = strLen;
            val[4].length = &len[4];
            val[4].is_null = &nullRes[4];

            val[5].buffer_type = MYSQL_TYPE_LONG;
            val[5].buffer = (char*)&song.year;
            val[5].length = &len[5];
            val[5].is_null = &nullRes[5];

            val[6].buffer_type = MYSQL_TYPE_LONG;
            val[6].buffer = (char*)&song.duration;
            val[6].length = &len[6];
            val[6].is_null = &nullRes[6];

            val[7].buffer_type = MYSQL_TYPE_LONG;
            val[7].buffer = (char*)&song.track;
            val[7].length = &len[7];
            val[7].is_null = &nullRes[7];

            val[8].buffer_type = MYSQL_TYPE_LONG;
            val[8].buffer = (char*)&song.disc;
            val[8].length = &len[8];
            val[8].is_null = &nullRes[8];

            val[9].buffer_type = MYSQL_TYPE_STRING;
            val[9].buffer = (char*)path;
            val[9].buffer_length = strLen;
            val[9].length = &len[9];
            val[9].is_null = &nullRes[9];

            val[10].buffer_type = MYSQL_TYPE_LONG;
            val[10].buffer = (char*)&song.coverArtId;
            val[10].length = &len[10];
            val[10].is_null = &nullRes[10];

            val[11].buffer_type = MYSQL_TYPE_LONG;
            val[11].buffer = (char*)&song.artistId;
            val[11].length = &len[11];
            val[11].is_null = &nullRes[11];

            val[12].buffer_type = MYSQL_TYPE_LONG;
            val[12].buffer = (char*)&song.albumId;;
            val[12].length = &len[12];
            val[12].is_null = &nullRes[12];

            val[13].buffer_type = MYSQL_TYPE_LONG;
            val[13].buffer = (char*)&song.genreId;
            val[13].length = &len[13];
            val[13].is_null = &nullRes[13];

            val[14].buffer_type = MYSQL_TYPE_LONG;
            val[14].buffer = (char*)&song.yearId;
            val[14].length = &len[14];
            val[14].is_null = &nullRes[14];

            status = ::mysql_stmt_bind_result(stmt, val);
            ::mysql_stmt_store_result(stmt);

            status = ::mysql_stmt_fetch(stmt);

            song.title = title;
            song.album = album;
            song.artist = artist;
            song.genre = genre;
            song.songPath = path;

        }
        status = ::mysql_stmt_next_result(stmt);
    }

    std::cout << "done parsing record" << std::endl;

    return song;
}
