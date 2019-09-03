#ifndef BASE_REPOSITORY_H_
#define BASE_REPOSITORY_H_

#include <string>

#include <mysql/mysql.h>

#include "models/models.h"

namespace Database
{
    class base_repository
    {
    public:
        base_repository();
        base_repository(const std::string&);
        base_repository(const Model::BinaryPath&);
    protected:
        MYSQL* setup_mysql_connection();
        MYSQL* setup_mysql_connection(Model::database_connection);

        MYSQL_RES* perform_mysql_query(MYSQL*, const std::string&);

        Model::database_connection details;
    private:
        void intitalizeDetails();
        void initializeDetails(const Model::BinaryPath&);

        std::string path;
        Model::BinaryPath m_binConf;
    };
}

#endif
