#ifndef BASE_REPOSITORY_H_
#define BASE_REPOSITORY_H_

#include <string>

#include <mysql/mysql.h>

#include "model/Models.h"

namespace database
{
    class BaseRepository
    {
    public:
        BaseRepository();
        BaseRepository(const std::string&);
        BaseRepository(const model::BinaryPath&);
    protected:
        MYSQL* setupMysqlConnection();
        MYSQL* setupMysqlConnection(model::DatabaseConnection);

        MYSQL_RES* performMysqlQuery(MYSQL*, const std::string&);

        model::DatabaseConnection details;
    private:
        void intitalizeDetails();
        void initializeDetails(const model::BinaryPath&);

        std::string path;
        model::BinaryPath m_binConf;
    };
}

#endif
