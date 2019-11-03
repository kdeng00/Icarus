#include "database/CoverArtRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>

namespace database {
CoverArtRepository::CoverArtRepository(const std::string& path) : BaseRepository(path)
{ }

CoverArtRepository::CoverArtRepository(const model::BinaryPath& bConf) : BaseRepository(bConf)
{ }


std::vector<model::Cover> CoverArtRepository::retrieveRecords()
{
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);
    const std::string query = "SELECT * FROM CoverArt";

    mysql_stmt_prepare(stmt, query.c_str(), query.size());
    mysql_stmt_execute(stmt);

    auto coverArts = parseRecords(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return coverArts;
}

model::Cover CoverArtRepository::retrieveRecord(model::Cover& cov, type::CoverFilter filter = type::CoverFilter::id)
{
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    qry << "SELECT * FROM CoverArt WHERE ";

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    auto songTitleLength = cov.songTitle.size();
    switch (filter) {
        case type::CoverFilter::id:
            qry << "CoverArtId = ?";

            params[0].buffer_type = MYSQL_TYPE_LONG;
            params[0].buffer = (char*)&cov.id;
            params[0].length = 0;
            params[0].is_null = 0;
            break;
        case type::CoverFilter::songTitle:
            qry << "SongTitle = ?";

            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char*)cov.songTitle.c_str();
            params[0].length = &songTitleLength;
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

    auto covDb = parseRecord(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);
    std::cout << "retrieved cover art record" << std::endl;

    return covDb;
}


bool CoverArtRepository::doesCoverArtExist(const model::Cover& cover, type::CoverFilter filter)
{
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    std::stringstream qry;
    qry << "SELECT * FROM CoverArt WHERE ";

    auto titleLength = cover.songTitle.size();
    switch (filter) {
        case type::CoverFilter::id:
            qry << "CoverArtId = ?";

            params[0].buffer_type = MYSQL_TYPE_LONG;
            params[0].buffer = (char*)&cover.id;
            params[0].length = 0;
            params[0].is_null = 0;
            break;
        case type::CoverFilter::songTitle:
            qry << "SongTitle = ?";

            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char*)cover.songTitle.c_str();
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

    mysql_stmt_store_result(stmt);
    auto rowCount = mysql_stmt_num_rows(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return (rowCount > 0) ? true : false;
}


// TODO: change to prepared statement
void CoverArtRepository::deleteRecord(const model::Cover& cov)
{
    auto conn = setupMysqlConnection();
    const std::string query("DELETE FROM CoverArt WHERE CoverArtId = " + std::to_string(cov.id));

    auto result = performMysqlQuery(conn, query);

    mysql_close(conn);
}

void CoverArtRepository::saveRecord(const model::Cover& cov)
{
    std::cout << "saving cover art record";
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    MYSQL_BIND params[2];
    memset(params, 0, sizeof(params));
    my_bool isNull;

    const std::string query = "INSERT INTO CoverArt(SongTitle, ImagePath) VALUES(?, ?)";

    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)cov.songTitle.c_str();
    auto songTitleLength = cov.songTitle.size();
    params[0].length = &songTitleLength;
    params[0].is_null = 0;

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)cov.imagePath.c_str();
    auto imagePathLength = cov.imagePath.size();
    params[1].length = &imagePathLength;
    params[1].is_null = 0;

    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    std::cout << "saved cover art record" << std::endl;
}

void CoverArtRepository::updateRecord(const model::Cover& cover)
{
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    qry << "UPDATE CoverArt SET ";
    qry << "SongTitle = ?, ";
    qry << "ImagePath = ? ";
    qry << "WHERE CoverArtId = ?";

    MYSQL_BIND params[3];
    memset(params, 0, sizeof(params));

    auto songTitleLength = cover.songTitle.size();
    auto imagePathLength = cover.imagePath.size();

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)cover.songTitle.c_str();
    params[0].length = &songTitleLength;
    params[0].is_null = 0;

    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)cover.imagePath.c_str();
    params[1].length = &imagePathLength;
    params[1].is_null = 0;

    params[2].buffer_type = MYSQL_TYPE_LONG;
    params[2].buffer = (char*)&cover.id;
    params[2].length = 0;
    params[2].is_null = 0;

    const std::string query = qry.str();
    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    std::cout << "updated cover art record" << std::endl;
}


std::vector<model::Cover> CoverArtRepository::parseRecords(MYSQL_STMT *stmt)
{
    mysql_stmt_store_result(stmt);
    auto rowCount = mysql_stmt_num_rows(stmt);
    std::cout << "number of results " << rowCount << std::endl;

    std::vector<model::Cover> coverArts;
    coverArts.reserve(rowCount);

    const auto valAmt = 3;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    for (auto status = 0; status == 0; status = mysql_stmt_next_result(stmt)) {
        if (mysql_stmt_field_count(stmt) > 0) {
            model::Cover coverArt;
            auto res = mysql_stmt_result_metadata(stmt);
            auto fields = mysql_fetch_fields(res);
            auto strLen = 1024;

            MYSQL_BIND val[valAmt];
            memset(val, 0, sizeof(val));

            char songTitle[strLen];
            char imagePath[strLen];

            val[0].buffer_type = MYSQL_TYPE_LONG;
            val[0].buffer = (char*)&coverArt.id;
            val[0].length = &len[0];
            val[0].is_null = &nullRes[0];

            val[1].buffer_type = MYSQL_TYPE_STRING;
            val[1].buffer = (char*)songTitle;
            val[1].buffer_length = strLen;
            val[1].length = &len[1];
            val[1].is_null = &nullRes[1];

            val[2].buffer_type = MYSQL_TYPE_STRING;
            val[2].buffer = (char*)imagePath;
            val[2].buffer_length = strLen;
            val[2].length = &len[2];
            val[2].is_null = &nullRes[2];

            status = mysql_stmt_bind_result(stmt, val);

            while (true) { 
                std::cout << "fetching statement result" << std::endl;
                status = mysql_stmt_fetch(stmt);

                if (status == 1 || status == MYSQL_NO_DATA) {
                    break;
                }

                coverArt.songTitle = songTitle;
                coverArt.imagePath = imagePath;

                coverArts.push_back(std::move(coverArt));
            }
        }
        std::cout << "fetching next result" << std::endl;
    }

    return coverArts;
}


model::Cover CoverArtRepository::parseRecord(MYSQL_RES *results)
{
    std::cout << "parsing record" << std::endl;
    model::Cover cov;
    auto fieldNum = mysql_num_fields(results);

    MYSQL_ROW row = mysql_fetch_row(results);

    for (auto i = 0; i != fieldNum; ++i) {
        switch (i) {
            case 0:
                cov.id = std::stoi(row[i]);
                break;
            case 1:
                cov.songTitle = row[i];
                break;
            case 2:
                cov.imagePath = row[i];
                break;
        }
    }
    std::cout << "done parsing record" << std::endl;

    return cov;
}

model::Cover CoverArtRepository::parseRecord(MYSQL_STMT *stmt)
{
    std::cout << "parsing cover art record" << std::endl;

    mysql_stmt_store_result(stmt);
    model::Cover cover;

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

            char songTitle[strLen];
            char imagePath[strLen];

            val[0].buffer_type = MYSQL_TYPE_LONG;
            val[0].buffer = (char*)&cover.id;
            val[0].length = &len[0];
            val[0].is_null = &nullRes[0];

            val[1].buffer_type = MYSQL_TYPE_STRING;
            val[1].buffer = (char*)&songTitle;
            val[1].buffer_length = strLen;
            val[1].length = &len[1];
            val[1].is_null = &nullRes[1];

            val[2].buffer_type = MYSQL_TYPE_STRING;
            val[2].buffer = (char*)&imagePath;
            val[2].buffer_length = strLen;
            val[2].length = &len[2];
            val[2].is_null = &nullRes[2];

            status = mysql_stmt_bind_result(stmt, val);
            mysql_stmt_store_result(stmt);

            status = mysql_stmt_fetch(stmt);

            cover.songTitle = songTitle;
            cover.imagePath = imagePath;
        }

        status = mysql_stmt_next_result(stmt);
    }

    std::cout << "done parsing cover art record" << std::endl;

    return cover;
}
}
