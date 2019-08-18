#ifndef BASE_REPOSITORY_H_
#define BASE_REPOSITORY_H_

#include<string>

#include <mysql/mysql.h>

#include "models.h"

class base_repository
{
public:
    MYSQL* setup_mysql_connection(database_connection);

    MYSQL_RES* perform_mysql_query(MYSQL*, std::string&);
private:
};

#endif
