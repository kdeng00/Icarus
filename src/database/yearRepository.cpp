#include "database/yearRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <cstring>


Database::yearRepository::yearRepository(const Model::BinaryPath& bConf)
    : base_repository(bConf)
{ }


Model::Year Database::yearRepository::retrieveRecord(Model::Year& year, Type::yearFilter filter)
{
    std::cout << "retrieving year record" << std::endl;
    std::stringstream qry;
    auto conn = setup_mysql_connection();
    qry << "SELECT yr.* FROM Year yr WHERE ";

    switch (filter) {
        case Type::yearFilter::id:
            qry << "yr.YearId = " << year.id;
            break;
        case Type::yearFilter::year:
            qry << "yr.Year = " << year.year;
            break;
        default:
            break;
    }

    qry << " ORDER BY yr.YearId DESC LIMIT 1";

    const auto query = qry.str();
    auto results = perform_mysql_query(conn, query);

    year = parseRecord(results);

    mysql_close(conn);

    std::cout << "retrieved record" << std::endl;

    return year;
}

void Database::yearRepository::saveRecord(const Model::Year& year)
{
    std::cout << "saving year record" << std::endl;

    auto conn = setup_mysql_connection();
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

Model::Year Database::yearRepository::parseRecord(MYSQL_RES *results)
{
    std::cout << "parsing year record" << std::endl;
    Model::Year year;

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
