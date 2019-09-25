#ifndef USERREPOSITORY_H_
#define USERREPOSITORY_H_

#include <iostream>
#include <memory>
#include <tuple>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/SaltFilter.h"
#include "type/UserFilter.h"

namespace database {
class UserRepository : BaseRepository {
public:
    UserRepository(const model::BinaryPath&);

    model::User retrieveUserRecord(model::User&, type::UserFilter);
    model::PassSec retrieverUserSaltRecord(model::PassSec&, type::SaltFilter);

    bool doesUserRecordExist(const model::User&, type::UserFilter);

    void saveUserRecord(const model::User&);
    void saveUserSalt(const model::PassSec&);
private:
    struct UserLengths;
    struct SaltLengths;

    struct UserLengths
    {
        unsigned long firstnameLength;
        unsigned long lastnameLength;
        unsigned long phoneLength;
        unsigned long emailLength;
        unsigned long usernameLength;
        unsigned long passwordLength;
    };
    struct SaltLengths
    {
        unsigned long saltLength;
    };

    std::shared_ptr<MYSQL_BIND> insertUserValues(const model::User&, std::shared_ptr<UserLengths>);
    std::shared_ptr<MYSQL_BIND> insertSaltValues(const model::PassSec&, std::shared_ptr<SaltLengths>);
    std::shared_ptr<MYSQL_BIND> valueBind(model::User&, std::tuple<char*, char*, char*, char*, char*, char*>&);
    std::shared_ptr<MYSQL_BIND> saltValueBind(model::PassSec&, char*);
    std::shared_ptr<UserLengths> fetchUserLengths(const model::User&);
    std::shared_ptr<SaltLengths> fetchSaltLengths(const model::PassSec&);

    std::tuple<char*, char*, char*, char*, char*, char*> fetchUV();

    model::User parseRecord(MYSQL_STMT*);
    model::PassSec parseSaltRecord(MYSQL_STMT*);
};
}


#endif
