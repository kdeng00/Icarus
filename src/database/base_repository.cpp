#include "database/base_repository.h"

MYSQL* base_repository::setup_mysql_connection(database_connection details)
{
    MYSQL *connection = mysql_init(NULL);

    // connect to the database with the details attached.
    if (!mysql_real_connect(connection,details.server.c_str(), details.username.c_str(), details.password.c_str(), details.database.c_str(), 0, NULL, 0)) {
      printf("Conection error : %s\n", mysql_error(connection));
      exit(1);
    }
    return connection;
}

MYSQL_RES* base_repository::perform_mysql_query(MYSQL *conn, const std::string& query)
{
   // send the query to the database
   if (mysql_query(conn, query.c_str()))
   {
      printf("MySQL query error : %s\n", mysql_error(conn));
      exit(1);
   }

   return mysql_use_result(conn);
}
