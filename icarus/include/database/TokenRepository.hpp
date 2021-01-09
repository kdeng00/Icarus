#ifndef TOKENREPOSITORY_H_
#define TOKENREPOSITORY_H_

#include <vector>
#include <utility>
#include <chrono>
#include <ctime>

#include <icarus_lib/icarus.h>

#include "database/BaseRepository.hpp"
#include "type/Filter.h"
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


        template<typename filter_type = type::TokenFilter>
        token retrieve_token(token &&tokn, filter_type filter = filter_type::token_id)
        {
            connection conn;
            create_connection<connection>(conn);

            auto rows = prepare_query<soci::row, soci::rowset, filter_type>(conn, tokn, filter);

            auto &r = rows.begin();
            auto t = parse_record<soci::row>(r);
            tokn.access_token = t.access_token;
            tokn.issued = t.issued;
            tokn.expires = t.expires;

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
            buff << "Issued, ";
	        buff << "Expires, UserId) VALUES(:access_token, ";
	        buff << ":originally_issued, :originally_expires, :user_id)";
	
	        const auto query = buff.str();

            using utility::General;
            auto func1 = General::convert_time_point_to_tm<std::chrono::system_clock::time_point,
                                                           std::tm>;
	
            auto issued = func1(tokn.issued);
            auto expires = func1(tokn.expires);
	
	        soci::statement stmt = (conn.prepare << query, soci::use(tokn.access_token, "access_token"),
	            soci::use(issued, "originally_issued"), soci::use(expires, "originally_expires"),
	            soci::use(usr.id, "user_id"));
	
	        stmt.execute();
	        auto rows_affected = stmt.get_affected_rows();

            conn.close();

            return rows_affected;
        }

        // Deletes Token record by id
        template<typename filter_type = type::TokenFilter, typename statement = soci::statement>
        int delete_token(const token &tokn, filter_type filter = filter_type::token_id)
        {
            connection conn;
            create_connection<connection>(conn);

            statement stmt;

            std::stringstream qry;
            qry << "DELETE FROM " << this->table_name << " WHERE ";

            switch(filter)
            {
            case filter_type::token_id:
                qry << "TokenId = :token_id";
                stmt = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"));
                break;
            case filter_type::access_token:
                qry << "AccessToken = :access_token";
                stmt = (conn.prepare << qry.str(), soci::use(tokn.access_token, "access_token"));
                break;
            case filter_type::user_id:
                qry << "UserId = :user_id";
                stmt = (conn.prepare << qry.str(), soci::use(tokn.user_id, "user_id"));
                break;
            case filter_type::token_and_user:
                qry << "TokenId = :token_id AND UserId = :user_id";
                stmt = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"),
                        soci::use(tokn.user_id, "user_id"));
                break;
            default:
                qry << "TokenId = :token_id";
                stmt = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"));
                break;
            }

            stmt.execute();
            const auto affected_rows = stmt.get_affected_rows();


            return affected_rows;
        }


        // Checks for the Token record's existence by id
        bool token_exists(const token &tokn)
        {
            return false;
        }
    private:
        template<typename row_item, template <typename> class row_container,
                 typename filter_type>
        auto prepare_query(const connection &conn, token &&tokn, filter_type filter)
        {
            std::stringstream qry;
            qry << "SELECT * FROM " << this->table_name << " WHERE ";


            row_container<row_item> some_rows;

            switch(filter)
            {
            case type::TokenFilter::id:
                qry << "TokenId = :token_id";
                some_rows = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"));
                break;
            case type::TokenFilter::access_token:
                qry << "AccessToken = :access_token";
                some_rows = (conn.prepare << qry.str(), soci::use(tokn.access_token, "access_token"));
                break;
            case type::TokenFilter::user_id:
                qry << "UserId = :user_id";
                some_rows = (conn.prepare << qry.str(), soci::use(tokn.user_id, "user_id"));
                break;
            case type::TokenFilter::token_and_user:
                qry << "TokenId = :token_id AND UserId = :user_id";
                some_rows = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"),
                        soci::use(tokn.user_id, "user_id"));
                break;
            default:
                qry << "TokenId = :token_id";
                some_rows = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"));
                break;
            }


            return some_rows;
        }

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
            auto orig_issued = row.template get<std::tm>("Issued", tm_default);
            auto orig_expires = row.template get<std::tm>("Expires", tm_default);
            tokn.refresh_token = row.template get<bool>("RefreshToken");
            tokn.issued = func1(orig_issued);
            tokn.expires = func1(orig_expires);
            tokn.active = row.template get<bool>("Active");
            tokn.user_id = row.template get<int>("UserId", int_default);

            return tokn;
        }
    };
}


#endif
