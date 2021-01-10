#ifndef TOKENREPOSITORY_H_
#define TOKENREPOSITORY_H_

#include <vector>
#include <memory>

#include <icarus_lib/icarus.h>

#include "database/BaseRepository.h"
#include "type/Filter.h"

namespace database
{
    class TokenRepository : public BaseRepository
    {
    public:
        TokenRepository() = delete;
        TokenRepository(const icarus_lib::binary_path &config);


        std::vector<icarus_lib::token> retrieve_all_tokens();


        icarus_lib::token retrieve_token(icarus_lib::token &tokn, type::TokenFilter filter = type::TokenFilter::id);


        int create_token(const icarus_lib::token &tokn, const icarus_lib::user &usr);

        // Deletes Token record by id
        int delete_token(const icarus_lib::token &tokn, type::TokenFilter filter = type::TokenFilter::id);


        // Checks for the Token record's existence by id
        bool token_exists(const icarus_lib::token &tokn);
    private:
        std::shared_ptr<soci::rowset<soci::row>> prepare_query(soci::session &conn, const icarus_lib::token &tokn, type::TokenFilter filter);

        icarus_lib::token parse_record(const soci::row &row);
    };
}


#endif
