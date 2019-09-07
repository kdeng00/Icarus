#include "database/GenreRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

database::GenreRepository::GenreRepository(const model::BinaryPath& bConf)
    : BaseRepository(bConf)
{ }


std::vector<model::Genre> database::GenreRepository::retrieveRecords()
{
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);
    const std::string query = "SELECT * FROM Genre";

    mysql_stmt_prepare(stmt, query.c_str(), query.size());
    mysql_stmt_execute(stmt);

    auto genres = parseRecords(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return genres;
}


model::Genre database::GenreRepository::retrieveRecord(model::Genre& genre, type::GenreFilter filter)
{
    // TODO: change to prepared statement
    
    std::cout << "retrieving genre record" << std::endl;
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    qry << "SELECT gnr.* FROM Genre gnr WHERE ";

    std::unique_ptr<char*> param;
    switch (filter) {
        case type::GenreFilter::id:
            qry << "gnr.GenreId = " << genre.id;
            break;
        case type::GenreFilter::category:
            param = std::make_unique<char*>(new char[genre.category.size()]);
            mysql_real_escape_string(conn, *param, genre.category.c_str(), genre.category.size());
            qry << "gnr.Category ='" << *param << "'";
            break;
        default:
            break;
    }

    qry << " ORDER BY GenreId DESC LIMIT 1";

    const auto query = qry.str();
    auto results = performMysqlQuery(conn, query);

    genre = parseRecord(results);

    mysql_close(conn);

    std::cout << "retrieved record" << std::endl;

    return genre;
}

bool database::GenreRepository::doesGenreExist(const model::Genre& genre, type::GenreFilter filter)
{
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    std::stringstream qry;
    qry << "SELECT * FROM Genre WHERE ";

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    auto categoryLength = genre.category.size();
    switch (filter) {
        case type::GenreFilter::id:
            qry << "GenreId = ?";

            params[0].buffer_type = MYSQL_TYPE_LONG;
            params[0].buffer = (char*)&genre.id;
            params[0].length = 0;
            params[0].is_null = 0;
            break;
        case type::GenreFilter::category:
            qry << "Category = ?";

            params[0].buffer_type = MYSQL_TYPE_STRING;
            params[0].buffer = (char*)genre.category.c_str();
            params[0].length = &categoryLength;
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

void database::GenreRepository::saveRecord(const model::Genre& genre)
{
    std::cout << "inserting genre record" << std::endl;

    auto conn = setupMysqlConnection();
    MYSQL_STMT *stmt = mysql_stmt_init(conn);

    const std::string query("INSERT INTO Genre(Category) VALUES(?)");

    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)genre.category.c_str();
    auto categoryLength = genre.category.size();
    params[0].length = &categoryLength;
    params[0].is_null = 0;

    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    std::cout << "inserted record" << std::endl;
}


std::vector<model::Genre> database::GenreRepository::parseRecords(MYSQL_STMT *stmt)
{
    mysql_stmt_store_result(stmt);

    std::vector<model::Genre> genres;
    genres.reserve(mysql_stmt_num_rows(stmt));

    if (mysql_stmt_field_count(stmt) == 0) {
        std::cout << "field count is 0" << std::endl;
        return genres;
    }

    model::Genre gnr;
    const auto valAmt = 2;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    auto res = mysql_stmt_result_metadata(stmt);
    auto fields = mysql_fetch_fields(res);
    const auto strLen = 1024;

    char category[strLen];

    MYSQL_BIND val[valAmt];
    memset(val, 0, sizeof(val));

    val[0].buffer_type = MYSQL_TYPE_LONG;
    val[0].buffer = (char*)&gnr.id;
    val[0].length = &len[0];
    val[0].is_null = &nullRes[0];

    val[1].buffer_type = MYSQL_TYPE_STRING;
    val[1].buffer = (char*)category;
    val[1].buffer_length = strLen;
    val[1].length = &len[1];
    val[1].is_null = &nullRes[1];

    for (auto status = mysql_stmt_bind_result(stmt, val); status == 0;) {
        std::cout << "fetching statement result" << std::endl;
        status = mysql_stmt_fetch(stmt);

        if (status == 0) {
            gnr.category = category;
            genres.push_back(std::move(gnr));
        }
    }

    return genres;
}

model::Genre database::GenreRepository::parseRecord(MYSQL_RES* results)
{
    std::cout << "parsing genre record" << std::endl;
    model::Genre genre;

    auto fieldNum = mysql_num_fields(results);
    auto row = mysql_fetch_row(results);

    for (auto i =0; i != fieldNum; ++i) {
        const std::string field(mysql_fetch_field(results)->name);

        if (field.compare("GenreId") == 0) {
            genre.id = std::stoi(row[i]);
        }
        if (field.compare("Category") == 0) {
            genre.category = row[i];
        }
    }

    std::cout << "parsed genre record" << std::endl;

    return genre;
}
model::Genre database::GenreRepository::parseRecord(MYSQL_STMT *stmt)
{
    // TODO: implement this
    
    model::Genre genre;

    return genre;
}
