#ifndef TOKEN_MANAGER_H_
#define TOKEN_MANAGER_H_

#include "models.h"

class token_manager
{
public:
    token_manager();

    loginResult retrieve_token();
    
    //LoginRes* retrieve_token(TokenReq *tok);
private:
    auth_credentials parse_auth_credentials();
};

#endif
