#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <iostream>

#include "dto/LoginResultDto.hpp"
#include "model/Models.h"

namespace manager {
class UserManager {
public:
    UserManager(const model::BinaryPath&);

    model::RegisterResult registerUser(model::User&);

    bool doesUserExist(const model::User&);
    bool validatePassword(const model::User&);

    void printUser(const model::User&);
private:
    model::BinaryPath m_bConf;
};
}

#endif
