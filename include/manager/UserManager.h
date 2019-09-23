#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <iostream>

#include "dto/LoginResultDto.hpp"
#include "model/Models.h"

namespace manager
{
    class UserManager
    {
    public:
        UserManager(const model::BinaryPath&);

        model::User userDtoConv(dto::UserDto::ObjectWrapper&);

        void printUser(const model::User&);
        void registerUser(model::User&);
    private:
        model::BinaryPath m_bConf;
    };
}

#endif
