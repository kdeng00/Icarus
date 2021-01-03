#ifndef TOKENREPOSITORY_H_
#define TOKENREPOSITORY_H_

#include <vector>

#include <icarus_lib/icarus.h>

#include "database/BaseRepository.hpp"

namespace database
{
    template<typename token, typename connection = soci::session>
    class TokenRepository : public BaseRepository
    {
    public:
        TokenRepository() = delete;
        TokenRepository(const icarus_lib::binary_path &config) : 
            BaseRepository(config)
        {
            this->table_name = "Token";
        }


        template<typename container = std::vector<token>>
        container retrieve_all_tokens()
        {
            connection conn;
            create_connection<connection>(conn);

            std::stringstream qry;
            qry << "SELECT * FROM " << this->table_name;

            soci::rowset<soci::row> rows = (conn.prepare << qry.str());

            container contr;

            // rows.get_affected_rows();
            for (auto &row : rows)
            {
                contr.emplace_back(parse_record<soci::row>(row));
            }

            conn.close();

            return contr;
        }


        template<typename user = icarus_lib::user>
        int create_token(const token &tokn, const user &usr)
        {
            return 0;
        }
    private:
        template<typename row_item>
        token parse_record(const row_item &row)
        {
            token tokn;

            return tokn;
        }
    };
}


#endif
