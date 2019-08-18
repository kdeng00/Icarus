#include <exception>
#include <iostream>
#include <filesystem>
#include <memory>
#include <string>

#include "models.h"
#include <mysql/mysql.h>
#include "oatpp/network/server/Server.hpp"
#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "appComponent.hpp"
#include "controller/loginController.hpp"
#include "database/base_repository.h"

namespace fs = std::filesystem;

void run(const std::string& working_path)
{
    appComponent component;

    //auto router = oatpp::web::server::HttpRouter::createShared();
    OATPP_COMPONENT(std::shared_ptr<oatpp::web::server::HttpRouter>, router);

    //router->route("GET", "/test", std::make_shared<loginHandler>());
    auto logController = std::make_shared<loginController>(working_path);
    logController->addEndpointsToRouter(router);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::server::ConnectionHandler>, connectionHandler);

    OATPP_COMPONENT(std::shared_ptr<oatpp::network::ServerConnectionProvider>, connectionProvider);

    oatpp::network::server::Server server(connectionProvider, connectionHandler);

    OATPP_LOGI("icarus", "Server running on port %s", connectionProvider->getProperty("port").getData());

    server.run();
}

MYSQL* mysql_connection_setup(database_connection mysql_details)
{
     // first of all create a mysql instance and initialize the variables within
    MYSQL *connection = mysql_init(NULL);

    // connect to the database with the details attached.
    if (!mysql_real_connect(connection,mysql_details.server.c_str(), mysql_details.username.c_str(), mysql_details.password.c_str(), mysql_details.database.c_str(), 0, NULL, 0)) {
      printf("Conection error : %s\n", mysql_error(connection));
      exit(1);
    }
    return connection;
}

MYSQL_RES* mysql_perform_query(MYSQL *connection, char *sql_query)
{
   // send the query to the database
   if (mysql_query(connection, sql_query))
   {
      printf("MySQL query error : %s\n", mysql_error(connection));
      exit(1);
   }

   return mysql_use_result(connection);
}

void test_database()
{
    database_connection mysqlD;
    mysqlD.server = "";  // where the mysql database is
    mysqlD.username = "";		// the root user of mysql
    mysqlD.password = ""; // the password of the root user in mysql
    mysqlD.database = "";	// the databse to pick

    base_repository base;
    auto conn = base.setup_mysql_connection(mysqlD);

    // assign the results return to the MYSQL_RES pointer
    const std::string query = "SELECT *, NULL FROM Song";
    auto res = base.perform_mysql_query(conn, query);
    auto num_of_fields = mysql_num_fields(res);

    printf("MySQL Tables in mysql database:\n");
    auto row_count = 1;
    
    for (MYSQL_ROW row = NULL; (row = mysql_fetch_row(res)) != NULL;) {
        std::cout << " row " << row_count << std::endl;
        for (auto i = 0; i != num_of_fields; ++i) {
            auto val = row[i];

            if ( val == NULL) {
                std::cout << "found null value " << std::endl;
                continue;
            }
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    /* clean up the database result set */
    mysql_free_result(res);
    /* clean up the database link */
    mysql_close(conn);
}


int main(int argc, char **argv)
{
    oatpp::base::Environment::init();
    std::string working_path = argv[0];

    test_database();
    run(working_path);

    oatpp::base::Environment::destroy();

    return 0;
}
