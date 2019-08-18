#ifndef TOKEN_MANAGER_H_
#define TOKEN_MANAGER_H_

#include <string_view>

#include "models.h"

class token_manager
{
public:
    token_manager();

    loginResult retrieve_token();
    loginResult retrieve_token(std::string_view);
private:
    auth_credentials parse_auth_credentials(std::string_view);
};

#endif
