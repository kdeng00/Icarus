#include "database/BaseRepository.h"

#include <iostream>

#include <nlohmann/json.hpp>

#include "manager/DirectoryManager.h"

namespace database {
    BaseRepository::BaseRepository(const std::string& path) : path(path) { 
        intitalizeDetails(); 
    }

    BaseRepository::BaseRepository(const icarus_lib::binary_path & bConf) {
        initializeDetails(bConf);
    }


    bool BaseRepository::testConnection() {
        auto conn = mysql_init(nullptr);
        if (!mysql_real_connect(conn, details.server.c_str(), details.username.c_str(), 
                details.password.c_str(), details.database.c_str(), 0, nullptr, 0)) {
            std::cout << "failed to connect to the database\n";
            return false;
        }

        mysql_close(conn);

        return true;
    }


    MYSQL* BaseRepository::setupMysqlConnection() {
        MYSQL *conn = mysql_init(nullptr);

        if (!mysql_real_connect(conn, details.server.c_str(), details.username.c_str(), 
                details.password.c_str(), details.database.c_str(), 0, nullptr, 0)) {
            std::cout << "connection error\n";
        }

        return conn;
    }

    MYSQL* BaseRepository::setupMysqlConnection(model::DatabaseConnection details) {
        MYSQL *connection = mysql_init(NULL);

        // connect to the database with the details attached.
        if (!mysql_real_connect(connection,details.server.c_str(), details.username.c_str(), 
                    details.password.c_str(), details.database.c_str(), 0, NULL, 0)) {
            std::cout << "Connection error: " << mysql_error(connection) << "\n";
            std::exit(-1);
        }
        return connection;
    }


    MYSQL_RES* BaseRepository::performMysqlQuery(MYSQL *conn, const std::string& query) {
        // send the query to the database
        if (mysql_query(conn, query.c_str())) {
            printf("MySQL query error : %s\n", mysql_error(conn));
            std::cout << "MySQL query error : " << mysql_error(conn) << "\n";
            std::exit(1);
        }

        return mysql_use_result(conn);
    }


    void BaseRepository::intitalizeDetails() {
        auto databaseConfig = manager::DirectoryManager::databaseConfigContent(path);
    
        details.database = databaseConfig["database"].get<std::string>();
        details.password = databaseConfig["password"].get<std::string>();
        details.server = databaseConfig["server"].get<std::string>();
        details.username = databaseConfig["username"].get<std::string>();
    }
    void BaseRepository::initializeDetails(const icarus_lib::binary_path & bConf)
    {
        auto databaseConfig = manager::DirectoryManager::databaseConfigContent(bConf);

        details.database = databaseConfig["database"].get<std::string>();
        details.server = databaseConfig["server"].get<std::string>();
        details.username = databaseConfig["username"].get<std::string>();
        details.password = databaseConfig["password"].get<std::string>();

        std::cout << "retrieved database details\n";
    }
}
