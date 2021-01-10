#ifndef BASE_REPOSITORY_H_
#define BASE_REPOSITORY_H_

#include <string>

#include <icarus_lib/icarus.h>
#include <mysql/mysql.h>
#include <soci/soci.h>

namespace database {
    class BaseRepository {
    public:
        BaseRepository() = default;
        BaseRepository(const std::string &path);
        BaseRepository(const std::string &path, std::string &&table_name);
        BaseRepository(const icarus_lib::binary_path &bConf);

        bool testConnection();
    protected:
        MYSQL *setupMysqlConnection();
        MYSQL *setupMysqlConnection(icarus_lib::database_connection details);


        MYSQL_RES* performMysqlQuery(MYSQL *conn, const std::string &query);


        void create_connection(soci::session &conn);


        std::string generate_connection_string() noexcept;

        icarus_lib::database_connection details;
        std::string table_name;
    private:
        void initializeDetails();
        void initializeDetails(const icarus_lib::binary_path &bConf);

        std::string path = "empty";
        icarus_lib::binary_path  m_binConf;
    };
}

#endif
