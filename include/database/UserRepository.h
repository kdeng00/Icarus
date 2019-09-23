#ifndef USERREPOSITORY_H_
#define USERREPOSITORY_H_

#include <iostream>
#include <memory>

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
        struct UserVals
        {
            /**
            std::shared_ptr<char*> firstname;
            std::shared_ptr<char*> lastname;
            std::shared_ptr<char*> phone;
            std::shared_ptr<char*> email;
            std::shared_ptr<char*> username;
            std::shared_ptr<char*> password;
            */
            char firstname[1024];
            char lastname[1024];
            char phone[1024];
            char email[1024];
            char username[1024];
            char password[1024];
        };

        std::shared_ptr<MYSQL_BIND> insertUserValues(const model::User&, std::shared_ptr<UserLengths>);
        std::shared_ptr<MYSQL_BIND> valueBind(model::User&, std::shared_ptr<UserVals>);
        std::shared_ptr<MYSQL_BIND> valueBind(model::User&, UserVals&);
        std::shared_ptr<MYSQL_BIND> valueBind(model::User&);
        std::shared_ptr<UserLengths> fetchUserLengths(const model::User&);
        std::shared_ptr<UserVals> fetchUserVals();

        model::User parseRecord(MYSQL_STMT*);
    };
}


#endif
