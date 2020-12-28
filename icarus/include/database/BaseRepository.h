#ifndef BASE_REPOSITORY_H_
#define BASE_REPOSITORY_H_

#include <string>

#include "icarus_lib/icarus.h"
#include <mysql/mysql.h>

#include "model/Models.h"

namespace database {
    class BaseRepository {
    public:
        BaseRepository() = default;
        BaseRepository(const std::string&);
        BaseRepository(const icarus_lib::binary_path &);

        bool testConnection();
    protected:
        MYSQL* setupMysqlConnection();
        MYSQL* setupMysqlConnection(model::DatabaseConnection);

        MYSQL_RES* performMysqlQuery(MYSQL*, const std::string&);

        model::DatabaseConnection details;
    private:
        void intitalizeDetails();
        void initializeDetails(const icarus_lib::binary_path &);

        std::string path;
        icarus_lib::binary_path  m_binConf;
    };
}

#endif
