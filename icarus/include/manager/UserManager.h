#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <iostream>

#include <icarus_lib/icarus.h>

#include "manager/BaseManager.hpp"

namespace manager {
    class UserManager : BaseManager<icarus_lib::binary_path>
    {
    public:
        UserManager(const icarus_lib::binary_path &b_conf);


        icarus_lib::register_result registerUser(icarus_lib::user &user);


        bool doesUserExist(const icarus_lib::user &user);
        bool validatePassword(const icarus_lib::user &user);


        void printUser(const icarus_lib::user &user);
    private:
    };
}

#endif
