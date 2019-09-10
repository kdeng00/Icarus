#include "database/YearRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>


database::YearRepository::YearRepository(const model::BinaryPath& bConf)
    : BaseRepository(bConf)
{ }


std::vector<model::Year> database::YearRepository::retrieveRecords()
{
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);
    const std::string query = "SELECT * FROM Year";

    mysql_stmt_prepare(stmt, query.c_str(), query.size());
    mysql_stmt_execute(stmt);

    auto yearRecs = parseRecords(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return yearRecs;
}

model::Year database::YearRepository::retrieveRecord(model::Year& year, type::YearFilter filter)
{
    // TODO: switch to prepared statements
    std::cout << "retrieving year record" << std::endl;
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    qry << "SELECT yr.* FROM Year yr WHERE ";

    switch (filter) {
        case type::YearFilter::id:
            qry << "yr.YearId = " << year.id;
            break;
        case type::YearFilter::year:
            qry << "yr.Year = " << year.year;
            break;
        default:
            break;
    }

    qry << " ORDER BY yr.YearId DESC LIMIT 1";

    const auto query = qry.str();
    auto results = performMysqlQuery(conn, query);

    year = parseRecord(results);

    mysql_close(conn);

    std::cout << "retrieved record" << std::endl;

    return year;
}

bool database::YearRepository::doesYearExist(const model::Year& year, type::YearFilter filter)
{
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    std::stringstream qry;
    qry << "SELECT * FROM Year WHERE ";

    MYSQL_BIND params[1];
    memset(params, 0, sizeof(params));

    switch (filter) {
        case type::YearFilter::id:
            qry << "YearId = ?";

            params[0].buffer_type = MYSQL_TYPE_LONG;
            params[0].buffer = (char*)&year.id;
            params[0].length = 0;
            params[0].is_null = 0;
            break;
        case type::YearFilter::year:
            qry << "Year = ?";

            params[0].buffer_type = MYSQL_TYPE_LONG;
            params[0].buffer = (char*)&year.year;
            params[0].length = 0;
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

void database::YearRepository::saveRecord(const model::Year& year)
{
    std::cout << "saving year record" << std::endl;

    auto conn = setupMysqlConnection();
    MYSQL_STMT *stmt = mysql_stmt_init(conn);

    const std::string query("INSERT INTO Year(Year) VALUES(?)");

    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());

    MYSQL_BIND params[1];
    memset(params, 0 , sizeof(params));

    params[0].buffer_type = MYSQL_TYPE_LONG;
    params[0].buffer = (char*)&year.year;
    params[0].length = 0;
    params[0].is_null = 0;

    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    std::cout << "saved record" << std::endl;
}


std::vector<model::Year> database::YearRepository::parseRecords(MYSQL_STMT *stmt)
{
    mysql_stmt_store_result(stmt);

    std::vector<model::Year> yearRecs;
    yearRecs.reserve(mysql_stmt_num_rows(stmt));

    if (mysql_stmt_field_count(stmt) == 0) {
        std::cout << "field count is 0" << std::endl;
        return yearRecs;
    }

    model::Year yearRec;
    const auto valAmt = 2;
    unsigned long len[valAmt];
    my_bool nullRes[valAmt];

    auto res = mysql_stmt_result_metadata(stmt);

    MYSQL_BIND val[valAmt];
    memset(val, 0, sizeof(val));

    val[0].buffer_type = MYSQL_TYPE_LONG;
    val[0].buffer = (char*)&yearRec.id;
    val[0].length = &len[0];
    val[0].is_null = &nullRes[0];

    val[1].buffer_type = MYSQL_TYPE_LONG;
    val[1].buffer = (char*)&yearRec.year;
    val[1].length = &len[1];
    val[1].is_null = &nullRes[1];

    for (auto status = mysql_stmt_bind_result(stmt, val); status == 0;) {
        std::cout << "fetching statement result" << std::endl;
        status = mysql_stmt_fetch(stmt);

        if (status == 0) {
            yearRecs.push_back(std::move(yearRec));
        }
    }

    return yearRecs;
}

model::Year database::YearRepository::parseRecord(MYSQL_RES *results)
{
    std::cout << "parsing year record" << std::endl;
    model::Year year;

    auto fieldNum = mysql_num_fields(results);
    auto row = mysql_fetch_row(results);

    for (auto i = 0; i != fieldNum; ++i) {
        const std::string field(mysql_fetch_field(results)->name);

        if (field.compare("YearId") == 0) {
            year.id= std::stoi(row[i]);
        }
        if (field.compare("Year") == 0) {
            year.year = std::stoi(row[i]);
        }
    }

    std::cout << "parse year record" << std::endl;

    return year;
}
model::Year database::YearRepository::parseRecord(MYSQL_STMT *stmt)
{
    // TODO: imeplement this
    // I really thought that I had already done this
    
    model::Year year;

    return year;
}
