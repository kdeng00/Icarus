#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <iostream>

#include "dto/LoginResultDto.hpp"
#include "model/Models.h"

namespace manager {
class UserManager
{
public:
    UserManager(const model::BinaryPath&);

    bool doesUserExist(const model::User&);
    bool validatePassword(const model::User&);

    void printUser(const model::User&);
    void registerUser(model::User&);

    static model::User userDtoConv(dto::UserDto::ObjectWrapper&);
private:
    model::BinaryPath m_bConf;
};
}

#endif
