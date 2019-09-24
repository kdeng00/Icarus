#ifndef USERREPOSITORY_H_
#define USERREPOSITORY_H_

#include <iostream>
#include <memory>
#include <tuple>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/UserFilter.h"

namespace database {
    class UserRepository : BaseRepository {
    public:
        UserRepository(const model::BinaryPath&);

        model::User retrieveUserRecord(model::User&, type::UserFilter);

        void saveUserRecord(const model::User&);
    private:
        struct UserLengths;
        struct UserLengths
        {
            unsigned long firstnameLength;
            unsigned long lastnameLength;
            unsigned long phoneLength;
            unsigned long emailLength;
            unsigned long usernameLength;
            unsigned long passwordLength;
        };

        std::shared_ptr<MYSQL_BIND> insertUserValues(const model::User&, std::shared_ptr<UserLengths>);
        std::shared_ptr<MYSQL_BIND> valueBind(model::User&, std::tuple<char*, char*, char*, char*, char*, char*>&);
        std::shared_ptr<UserLengths> fetchUserLengths(const model::User&);

        std::tuple<char*, char*, char*, char*, char*, char*> fetchUV();

        model::User parseRecord(MYSQL_STMT*);
    };
}


#endif
