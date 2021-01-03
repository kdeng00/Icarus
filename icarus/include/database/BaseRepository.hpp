#ifndef BASE_REPOSITORY_H_
#define BASE_REPOSITORY_H_

#include <iostream>
#include <string>

#include <icarus_lib/icarus.h>
#include <mysql/mysql.h>
#include <soci/soci.h>
#include <soci/mysql/soci-mysql.h>

#include "manager/DirectoryManager.h"

namespace database {
    class BaseRepository {
    public:
        BaseRepository() = default;
        BaseRepository(const std::string &path) : path(path)
        {
        }
        BaseRepository(const std::string &path, std::string &&table_name) : 
            path(path), table_name(table_name)
        {
            initializeDetails();
        }
        BaseRepository(const icarus_lib::binary_path &bConf) : m_binConf(bConf)
        {
            initializeDetails();
        }

        bool testConnection()
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
    protected:
        MYSQL *setupMysqlConnection()
        {
	        MYSQL *conn = mysql_init(nullptr);
	
	        if (!mysql_real_connect(conn, details.server.c_str(), details.username.c_str(), 
	                details.password.c_str(), details.database.c_str(), 0, nullptr, 0)) {
	            std::cout << "connection error\n";
	        }
	
	        return conn;
        }

        MYSQL *setupMysqlConnection(icarus_lib::database_connection details)
        {
            MYSQL *connection = mysql_init(nullptr);

	        if (!mysql_real_connect(connection, details.server.c_str(), details.username.c_str(), 
	                details.password.c_str(), details.database.c_str(), 0, nullptr, 0)) {
	            std::cout << "connection error\n";
	        }

            return connection;
        }

        /**
	    MYSQL* BaseRepository::setupMysqlConnection(icarus_lib::database_connection details) {
	        MYSQL *connection = mysql_init(NULL);
	
	        // connect to the database with the details attached.
	        if (!mysql_real_connect(connection,details.server.c_str(), details.username.c_str(), 
	                    details.password.c_str(), details.database.c_str(), 0, NULL, 0)) {
	            std::cout << "Connection error: " << mysql_error(connection) << "\n";
	            std::exit(-1);
	        }
	        return connection;
	        }
	    */

        MYSQL_RES* performMysqlQuery(MYSQL *conn, const std::string &query)
        {
	        // send the query to the database
	        if (mysql_query(conn, query.c_str())) {
	            printf("MySQL query error : %s\n", mysql_error(conn));
	            std::cout << "MySQL query error : " << mysql_error(conn) << "\n";
	            std::exit(1);
	        }
	
	        return mysql_use_result(conn);
        }


        template<typename connection>
        void create_connection(connection &conn)
        {
            auto conn_string = generate_connection_string<std::string>();
            conn.open(soci::mysql, conn_string);
        }


        template<typename str_type>
        str_type generate_connection_string()
        {
            std::stringstream qry;
            qry << "db=" << this->details.database << " user=";
            qry << this->details.username << " host=" << this->details.server;
            qry << " password='" << this->details.password << "'";

            return qry.str();
        }

        icarus_lib::database_connection details;
        std::string table_name;
    private:
        void initializeDetails()
        {
	        auto databaseConfig = manager::DirectoryManager::databaseConfigContent(m_binConf);
	    
	        details.database = databaseConfig["database"].get<std::string>();
	        details.password = databaseConfig["password"].get<std::string>();
	        details.server = databaseConfig["server"].get<std::string>();
	        details.username = databaseConfig["username"].get<std::string>();
        }
        void initializeDetails(const icarus_lib::binary_path &bConf)
        {
	        auto databaseConfig = manager::DirectoryManager::databaseConfigContent(bConf);
	
	        details.database = databaseConfig["database"].get<std::string>();
	        details.server = databaseConfig["server"].get<std::string>();
	        details.username = databaseConfig["username"].get<std::string>();
	        details.password = databaseConfig["password"].get<std::string>();
	
	        std::cout << "retrieved database details\n";
        }

        std::string path = "empty";
        icarus_lib::binary_path  m_binConf;
    };
}

#endif
