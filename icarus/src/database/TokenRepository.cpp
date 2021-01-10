#include "database/TokenRepository.h"

#include <string>
#include <ctime>
#include <chrono>
#include <utility>

#include "utility/General.hpp"

using std::vector;
using std::string;
using std::stringstream;
using std::chrono::system_clock;
using std::tm;
using std::shared_ptr;

using icarus_lib::binary_path;
using icarus_lib::token;
using icarus_lib::user;
using soci::rowset;
using soci::session;
using soci::statement;

using utility::General;
using type::TokenFilter;

namespace database
{
    TokenRepository::TokenRepository(const binary_path &config) : 
        BaseRepository(config)
    {
        this->table_name = "Token";
    }

    
    vector<token> TokenRepository::retrieve_all_tokens()
    {
        session conn;
        create_connection(conn);

        stringstream qry;
        qry << "SELECT * FROM " << this->table_name;

        soci::rowset<soci::row> rows = (conn.prepare << qry.str());

        vector<token> contr;

        for (auto &row : rows)
        {
            contr.emplace_back(parse_record(row));
        }

        conn.close();

        return contr;
    }


    token TokenRepository::retrieve_token(token &tokn, TokenFilter filter)
    {
        session conn;
        create_connection(conn);

        shared_ptr<rowset<soci::row>> rows = prepare_query(conn, tokn, filter);

        for (auto &row : *rows)
        {
            auto fetched = parse_record(row);
            tokn.access_token = fetched.access_token;
            tokn.issued = fetched.issued;
            tokn.expires = fetched.expires;

            break;
        }


        conn.close();


        return tokn;
    }


    int TokenRepository::create_token(const token &tokn, const user &usr)
    {
        session conn;
        create_connection(conn);

        stringstream buff;
        buff << "INSERT INTO " << this->table_name << " (AccessToken, ";
        buff << "Issued, ";
        buff << "Expires, UserId) VALUES(:access_token, ";
        buff << ":originally_issued, :originally_expires, :user_id)";

        const auto query = buff.str();

        auto func1 = General::convert_time_point_to_tm<system_clock::time_point,
                                                       tm>;

        auto issued = func1(tokn.issued);
        auto expires = func1(tokn.expires);

        statement stmt = (conn.prepare << query, soci::use(tokn.access_token, "access_token"),
            soci::use(issued, "originally_issued"), soci::use(expires, "originally_expires"),
            soci::use(usr.id, "user_id"));

        stmt.execute();
        auto rows_affected = stmt.get_affected_rows();

        conn.close();

        return rows_affected;
    }

    // Deletes Token record by id
    int TokenRepository::delete_token(const token &tokn, TokenFilter filter)
    {
        session conn;
        create_connection(conn);

        shared_ptr<statement> stmt;

        stringstream qry;
        qry << "DELETE FROM " << this->table_name << " WHERE ";

        switch(filter)
        {
            case TokenFilter::id:
            {
                qry << "TokenId = :token_id";
                auto stt = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"));
                stmt = std::make_shared<statement>(stt);
                break;
            }
            case TokenFilter::access_token:
            {
                qry << "AccessToken = :access_token";
                auto stt = (conn.prepare << qry.str(), soci::use(tokn.access_token, "access_token"));
                stmt = std::make_shared<statement>(stt);
                break;
            }
            case TokenFilter::user_id:
            {
                qry << "UserId = :user_id";
                auto stt = (conn.prepare << qry.str(), soci::use(tokn.user_id, "user_id"));
                stmt = std::make_shared<statement>(stt);
                break;
            }
            case TokenFilter::token_and_user:
            {
                qry << "TokenId = :token_id AND UserId = :user_id";
                auto stt = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"),
                        soci::use(tokn.user_id, "user_id"));
                stmt = std::make_shared<statement>(stt);
                break;
            }
            default:
            {
                qry << "TokenId = :token_id";
                auto stt = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"));
                stmt = std::make_shared<statement>(stt);
                break;
            }
        }

        stmt->execute();
        const auto affected_rows = stmt->get_affected_rows();


        return affected_rows;
    }


    bool TokenRepository::token_exists(const token &tokn)
    {
        return false;
    }


    shared_ptr<soci::rowset<soci::row>> TokenRepository::prepare_query(session &conn, const token &tokn, TokenFilter filter)
    {
        stringstream qry;
        qry << "SELECT * FROM " << this->table_name << " WHERE ";


        auto d = std::shared_ptr<soci::rowset<soci::row>>(nullptr);

        switch(filter)
        {
            case type::TokenFilter::id:
            {
                qry << "TokenId = :token_id";
                rowset<soci::row> some_rows = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"));
                d = std::make_shared<rowset<soci::row>>(some_rows);
                break;
            }
            case type::TokenFilter::access_token:
            {
                qry << "AccessToken = :access_token";
                rowset<soci::row> some_rows = (conn.prepare << qry.str(), soci::use(tokn.access_token, "access_token"));
                d = std::make_shared<rowset<soci::row>>(some_rows);
                break;
            }
            case type::TokenFilter::user_id:
            {
                qry << "UserId = :user_id";
                rowset<soci::row> some_rows = (conn.prepare << qry.str(), soci::use(tokn.user_id, "user_id"));
                d = std::make_shared<rowset<soci::row>>(some_rows);
                break;
            }
            case type::TokenFilter::token_and_user:
            {
                qry << "TokenId = :token_id AND UserId = :user_id";
                rowset<soci::row> some_rows = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"),
                        soci::use(tokn.user_id, "user_id"));
                d = std::make_shared<rowset<soci::row>>(some_rows);
                break;
            }
            default:
            {
                qry << "TokenId = :token_id";
                rowset<soci::row> some_rows = (conn.prepare << qry.str(), soci::use(tokn.token_id, "token_id"));
                d = std::make_shared<rowset<soci::row>>(some_rows);
                break;
            }
        }


        return d;
    }

    token TokenRepository::parse_record(const soci::row &row)
    {
        tm tm_default;
        string str_default("none");
        int int_default = 0;

        auto func1 = General::convert_tm_to_time_point<system_clock::time_point, tm>;

        token tokn;
        tokn.token_id = row.template get<int>("TokenId");
        tokn.access_token = row.template get<string>("AccessToken", str_default);
        auto orig_issued = row.template get<tm>("Issued", tm_default);
        auto orig_expires = row.template get<tm>("Expires", tm_default);
        tokn.refresh_token = row.template get<bool>("RefreshToken");
        tokn.issued = func1(orig_issued);
        tokn.expires = func1(orig_expires);
        tokn.active = row.template get<bool>("Active");
        tokn.user_id = row.template get<int>("UserId", int_default);

        return tokn;
    }
}
