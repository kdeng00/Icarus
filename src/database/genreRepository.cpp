#include "database/genreRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

Database::genreRepository::genreRepository(const Model::BinaryPath& bConf)
    : base_repository(bConf)
{ }


Model::Genre Database::genreRepository::retrieveRecord(Model::Genre& genre, Type::genreFilter filter)
{
    std::cout << "retrieving genre record" << std::endl;
    std::stringstream qry;
    auto conn = setup_mysql_connection();
    qry << "SELECT gnr.* FROM Genre gnr WHERE ";

    std::unique_ptr<char*> param;
    switch (filter) {
        case Type::genreFilter::id:
            qry << "gnr.GenreId = " << genre.id;
            break;
        case Type::genreFilter::category:
            param = std::make_unique<char*>(new char[genre.category.size()]);
            mysql_real_escape_string(conn, *param, genre.category.c_str(), genre.category.size());
            qry << "gnr.Category ='" << *param << "'";
            break;
        default:
            break;
    }

    qry << " ORDER BY GenreId DESC LIMIT 1";

    const auto query = qry.str();
    auto results = perform_mysql_query(conn, query);

    genre = parseRecord(results);

    mysql_close(conn);

    std::cout << "retrieved record" << std::endl;

    return genre;
}

void Database::genreRepository::saveRecord(const Model::Genre& genre)
{
    std::cout << "inserting genre record" << std::endl;

    auto conn = setup_mysql_connection();
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

Model::Genre Database::genreRepository::parseRecord(MYSQL_RES* results)
{
    std::cout << "parsing genre record" << std::endl;
    Model::Genre genre;

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
