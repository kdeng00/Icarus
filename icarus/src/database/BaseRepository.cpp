#include "database/BaseRepository.h"
#include "icarus_lib/icarus.h"

#include <iostream>
#include <sstream>

#include <soci/mysql/soci-mysql.h>

#include "manager/DirectoryManager.h"

using std::string;
using std::stringstream;

using icarus_lib::database_connection;
using icarus_lib::binary_path;
using soci::session;

using manager::DirectoryManager;

namespace database
{

    BaseRepository::BaseRepository(const string &path) : path(path)
    {
    }
    BaseRepository::BaseRepository(const string &path, string &&table_name) : 
        path(path), table_name(table_name)
    {
        initializeDetails();
    }
    BaseRepository::BaseRepository(const binary_path &bConf) : m_binConf(bConf)
    {
        initializeDetails();
    }
    
    bool BaseRepository::testConnection()
    {
        auto conn = mysql_init(nullptr);

        if (!mysql_real_connect(conn, details.server.c_str(), details.username.c_str(), 
                details.password.c_str(), details.database.c_str(), 0, nullptr, 0)) {
            std::cout << "failed to connect to the database\n";
            return false;
        }

        mysql_close(conn);

        return true;
    }

    MYSQL *BaseRepository::setupMysqlConnection()
    {
        MYSQL *conn = mysql_init(nullptr);

        if (!mysql_real_connect(conn, details.server.c_str(), details.username.c_str(), 
                details.password.c_str(), details.database.c_str(), 0, nullptr, 0)) {
            std::cout << "connection error\n";
        }

        return conn;
    }

    MYSQL *BaseRepository::setupMysqlConnection(icarus_lib::database_connection details)
    {
        MYSQL *connection = mysql_init(nullptr);

        if (!mysql_real_connect(connection, details.server.c_str(), details.username.c_str(), 
                details.password.c_str(), details.database.c_str(), 0, nullptr, 0)) {
            std::cout << "connection error\n";
        }

        return connection;
    }

    MYSQL_RES* BaseRepository::performMysqlQuery(MYSQL *conn, const string &query)
    {
        // send the query to the database
        if (mysql_query(conn, query.c_str())) {
            printf("MySQL query error : %s\n", mysql_error(conn));
            std::cout << "MySQL query error : " << mysql_error(conn) << "\n";
            std::exit(1);
        }

        return mysql_use_result(conn);
    }


    void BaseRepository::create_connection(session &conn)
    {
        auto conn_string = generate_connection_string();
        conn.open(soci::mysql, conn_string);
    }


    string BaseRepository::generate_connection_string() noexcept
    {
        stringstream qry;
        qry << "db=" << this->details.database << " user=";
        qry << this->details.username << " host=" << this->details.server;
        qry << " password='" << this->details.password << "'";

        return qry.str();
    }


    void BaseRepository::initializeDetails()
    {
        auto databaseConfig = DirectoryManager::databaseConfigContent(m_binConf);
    
        details.database = databaseConfig["database"].get<string>();
        details.password = databaseConfig["password"].get<string>();
        details.server = databaseConfig["server"].get<string>();
        details.username = databaseConfig["username"].get<string>();
    }
    void BaseRepository::initializeDetails(const binary_path &bConf)
    {
        auto databaseConfig = DirectoryManager::databaseConfigContent(bConf);

        details.database = databaseConfig["database"].get<string>();
        details.server = databaseConfig["server"].get<string>();
        details.username = databaseConfig["username"].get<string>();
        details.password = databaseConfig["password"].get<string>();

        std::cout << "retrieved database details\n";
    }
}
