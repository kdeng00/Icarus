#ifndef USERREPOSITORY_H_
#define USERREPOSITORY_H_

#include <iostream>
#include <memory>
#include <tuple>

#include "icarus_lib/icarus.h"

#include "database/BaseRepository.h"
#include "type/SaltFilter.h"
#include "type/UserFilter.h"

namespace database {
    class UserRepository : BaseRepository {
    public:
        UserRepository(const icarus_lib::binary_path &);

        icarus_lib::user retrieveUserRecord(icarus_lib::user&, type::UserFilter);
        icarus_lib::pass_sec retrieverUserSaltRecord(icarus_lib::pass_sec&, type::SaltFilter);

        bool doesUserRecordExist(const icarus_lib::user&, type::UserFilter);

        void saveUserRecord(const icarus_lib::user&);
        void saveUserSalt(const icarus_lib::pass_sec&);
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

        std::shared_ptr<MYSQL_BIND> insertUserValues(const icarus_lib::user&, 
                std::shared_ptr<UserLengths>);
        std::shared_ptr<MYSQL_BIND> insertSaltValues(const icarus_lib::pass_sec&, 
                std::shared_ptr<SaltLengths>);
        std::shared_ptr<MYSQL_BIND> valueBind(icarus_lib::user&, 
                std::tuple<char*, char*, char*, char*, char*, char*>&);
        std::shared_ptr<MYSQL_BIND> saltValueBind(icarus_lib::pass_sec&, char*);
        std::shared_ptr<UserLengths> fetchUserLengths(const icarus_lib::user&);
        std::shared_ptr<SaltLengths> fetchSaltLengths(const icarus_lib::pass_sec&);

        std::tuple<char*, char*, char*, char*, char*, char*> fetchUV();

        icarus_lib::user parseRecord(MYSQL_STMT*);
        icarus_lib::pass_sec parseSaltRecord(MYSQL_STMT*);
    };
}

#endif
