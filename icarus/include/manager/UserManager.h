#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <iostream>

#include "icarus_lib/icarus.h"

#include "dto/LoginResultDto.hpp"

namespace manager {
    class UserManager {
    public:
        UserManager(const icarus_lib::binary_path &);

        icarus_lib::register_result registerUser(icarus_lib::user&);

        bool doesUserExist(const icarus_lib::user&);
        bool validatePassword(const icarus_lib::user&);

        void printUser(const icarus_lib::user&);
    private:
        icarus_lib::binary_path  m_bConf;
    };
}

#endif
