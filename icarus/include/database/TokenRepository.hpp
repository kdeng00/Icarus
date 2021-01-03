#ifndef TOKENREPOSITORY_H_
#define TOKENREPOSITORY_H_

#include <vector>
#include <utility>
#include <chrono>
#include <ctime>

#include <icarus_lib/icarus.h>

#include "database/BaseRepository.hpp"
#include "utility/General.hpp"

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

            for (auto &row : rows)
            {
                contr.emplace_back(parse_record<soci::row>(row));
            }

            conn.close();

            return contr;
        }


        token retrieve_token(token &&tokn)
        {
            connection conn;
            create_connection<connection>(conn);

            std::stringstream qry;
            qry << "SELECT * FROM " << this->table_name;
            qry << " WHERE " << this->table_name << "Id";
            qry << " = :token_id LIMIT 1";

            soci::rowset<soci::row> rows = (conn.prepare << qry.str(),
                    soci::use(tokn.token_id, "token_id"));

            auto r = rows.begin();
            auto t = parse_record<soci::row>(r);
            tokn.access_token = t.access_token;
            tokn.orginally_issued = t.originally_issued;
            tokn.originally_expires = t.originally_expires;
            tokn.refresh_count = t.refresh_count;

            conn.close();


            return std::forward(tokn);
        }


        template<typename user = icarus_lib::user>
        int create_token(const token &tokn, const user &usr)
        {
            connection conn;
            create_connection<connection>(conn);

            std::stringstream buff;
	        buff << "INSERT INTO " << this->table_name << " (AccessToken, ";
            buff << "OriginallyIssued, ";
	        buff << "OriginallyExpires, UserId) VALUES(:access_token, ";
	        buff << ":originally_issued, :originally_expires, :user_id)";
	
	        const auto query = buff.str();

            using utility::General;
            auto func1 = General::convert_time_point_to_tm<std::chrono::system_clock::time_point,
                                                           std::tm>;
	
            auto issued = func1(tokn.originally_issued);
            auto expires = func1(tokn.originally_expires);
	
	        soci::statement stmt = (conn.prepare << query, soci::use(tokn.access_token, "access_token"),
	            soci::use(issued, "originally_issued"),
	            soci::use(expires, "originally_expires"),
	            soci::use(usr.id, "user_id"));
	
	        stmt.execute();
	        auto rows_affected = stmt.get_affected_rows();

            conn.close();

            return rows_affected;
        }

        // Deletes Token record by id
        int delete_token(const token &tokn)
        {
            return 0;
        }


        // Checks for the Token record's existence by id
        bool token_exists(const token &tokn)
        {
            return false;
        }
    private:
        template<typename row_item>
        token parse_record(const row_item &row)
        {
            std::tm tm_default;
            std::string str_default("none");
            int int_default = 0;

            using utility::General;
            auto func1 = General::convert_tm_to_time_point<std::chrono::system_clock::time_point,
                                                           std::tm>;

            token tokn;
            tokn.token_id = row.template get<int>("TokenId");
            tokn.access_token = row.template get<std::string>("AccessToken", str_default);
            auto orig_issued = row.template get<std::tm>("OriginallyIssued", tm_default);
            auto orig_expires = row.template get<std::tm>("OriginallyExpires", tm_default);
            auto refresh_issued = row.template get<std::tm>("RefreshedIssued", tm_default);
            auto refresh_expires = row.template get<std::tm>("RefreshedExpires", tm_default);
            tokn.originally_issued = func1(orig_issued);
            tokn.originally_expires = func1(orig_expires);
            tokn.refresh_issued = func1(refresh_issued);
            tokn.refresh_expires = func1(refresh_expires);
            tokn.refresh_count = row.template get<int>("RefreshCount", int_default);
            tokn.active = row.template get<bool>("Active");
            tokn.user_id = row.template get<int>("UserId", int_default);

            return tokn;
        }
    };
}


#endif
