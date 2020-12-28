#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <iostream>

#include "icarus_lib/icarus.h"

#include "dto/LoginResultDto.hpp"
#include "model/Models.h"

namespace manager {
    class UserManager {
    public:
        UserManager(const icarus_lib::binary_path &);

        model::RegisterResult registerUser(model::User&);

        bool doesUserExist(const model::User&);
        bool validatePassword(const model::User&);

        void printUser(const model::User&);
    private:
        icarus_lib::binary_path  m_bConf;
    };
}

#endif
