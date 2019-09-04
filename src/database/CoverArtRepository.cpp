#include "database/CoverArtRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

database::CoverArtRepository::CoverArtRepository(const std::string& path) : BaseRepository(path)
{ }

database::CoverArtRepository::CoverArtRepository(const model::BinaryPath& bConf) : BaseRepository(bConf)
{ }

model::Cover database::CoverArtRepository::retrieveRecord(model::Cover& cov, type::CoverFilter filter = type::CoverFilter::id)
{
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    qry << "SELECT * FROM CoverArt WHERE ";

    std::unique_ptr<char*> param;
    switch (filter) {
        case type::CoverFilter::id:
            qry << "CoverArtId = " << cov.id;
            break;
        case type::CoverFilter::songTitle:
            param = std::make_unique<char*>(new char[cov.songTitle.size()]);
            mysql_real_escape_string(conn, *param, cov.songTitle.c_str(), cov.songTitle.size());
            qry << "SongTitle = '" << *param << "'";
            break;
        case type::CoverFilter::imagePath:
            param = std::make_unique<char*>(new char[cov.imagePath.size()]);
            mysql_real_escape_string(conn, *param, cov.imagePath.c_str(), cov.imagePath.size());
            qry << "ImagePath = '" << *param << "'";
            break;
    }

    const std::string query = qry.str();
    auto results = performMysqlQuery(conn, query);
    std::cout << "the query has been performed" << std::endl;

    auto covDb = parseRecord(results);

    mysql_close(conn);
    std::cout << "done" << std::endl;

    return covDb;
}

bool database::CoverArtRepository::doesCoverArtExist(const model::Cover& cover, type::CoverFilter filter)
{
    // TODO: implement this
    return false;
}

void database::CoverArtRepository::deleteRecord(const model::Cover& cov)
{
    auto conn = setupMysqlConnection();
    const std::string query("DELETE FROM CoverArt WHERE CoverArtId = " + std::to_string(cov.id));

    auto result = performMysqlQuery(conn, query);

    mysql_close(conn);
}

void database::CoverArtRepository::saveRecord(const model::Cover& cov)
{
    auto conn = setupMysqlConnection();

    MYSQL_STMT *stmt;
    MYSQL_BIND params[2];
    my_bool isNull;
    int status;

    stmt = mysql_stmt_init(conn);

    const std::string query = "INSERT INTO CoverArt(SongTitle, ImagePath) VALUES(?, ?)";

    status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

    memset(params, 0, sizeof(params));

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

    std::cout << "done" << std::endl;
}

model::Cover database::CoverArtRepository::parseRecord(MYSQL_RES *results)
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

model::Cover database::CoverArtRepository::parseRecord(MYSQL_STMT *stmt)
{
    // TODO: implement this
    
    model::Cover cover;

    return cover;
}
