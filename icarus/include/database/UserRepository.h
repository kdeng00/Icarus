#ifndef USERREPOSITORY_H_
#define USERREPOSITORY_H_

#include <memory>
#include <tuple>

#include "icarus_lib/icarus.h"

#include "database/BaseRepository.h"
#include "type/SaltFilter.h"
#include "type/UserFilter.h"

namespace database {
    class UserRepository : BaseRepository {
    public:
        UserRepository() = default;
        UserRepository(const icarus_lib::binary_path &config);


        icarus_lib::user retrieveUserRecord(icarus_lib::user &user, 
                                    type::UserFilter filter = type::UserFilter::username);
        icarus_lib::pass_sec retrieverUserSaltRecord(icarus_lib::pass_sec &userSec, type::SaltFilter filter);

        [[deprecated("Use user_exists() function instead")]]
        bool doesUserRecordExist(const icarus_lib::user &user, type::UserFilter filter);

        bool user_exists(const icarus_lib::user &user, type::UserFilter filter = type::UserFilter::username);

        void saveUserRecord(const icarus_lib::user &user);
        void saveUserSalt(const icarus_lib::pass_sec &userSec);
    private:
        struct UserLengths;
        struct SaltLengths;

        struct UserLengths {
            unsigned long firstnameLength;
            unsigned long lastnameLength;
            unsigned long phoneLength;
            unsigned long emailLength;
            unsigned long usernameLength;
            unsigned long passwordLength;
        };
        struct SaltLengths {
            unsigned long saltLength;
        };

        std::shared_ptr<MYSQL_BIND> insertUserValues(const icarus_lib::user &user, 
                std::shared_ptr<UserLengths> lengths);
        std::shared_ptr<MYSQL_BIND> insertSaltValues(const icarus_lib::pass_sec &passSec, 
                std::shared_ptr<SaltLengths> lengths);
        std::shared_ptr<MYSQL_BIND> valueBind(icarus_lib::user &user, 
                std::tuple<char*, char*, char*, char*, char*, char*> &us);
        std::shared_ptr<MYSQL_BIND> saltValueBind(icarus_lib::pass_sec &userSalt, char *salt);
        std::shared_ptr<UserLengths> fetchUserLengths(const icarus_lib::user &user);
        std::shared_ptr<SaltLengths> fetchSaltLengths(const icarus_lib::pass_sec &pass_sec);

        std::tuple<char*, char*, char*, char*, char*, char*> fetchUV();

        icarus_lib::user parseRecord(MYSQL_STMT *stmt);

        icarus_lib::pass_sec parseSaltRecord(MYSQL_STMT *stmt);
    };
}

#endif
