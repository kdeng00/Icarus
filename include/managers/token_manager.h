#ifndef TOKEN_MANAGER_H_
#define TOKEN_MANAGER_H_

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <jwt-cpp/jwt.h>

#include "models/models.h"
#include "types/scopes.h"

class token_manager
{
public:
    token_manager();

    Model::loginResult retrieve_token();
    Model::loginResult retrieve_token(std::string_view);

    bool is_token_valid(std::string&, Scope);
private:
    Model::auth_credentials parse_auth_credentials(std::string_view);

    std::vector<std::string> extract_scopes(const jwt::decoded_jwt&&);
    std::pair<bool, std::vector<std::string>> fetch_auth_header(const std::string&);

    bool token_supports_scope(const std::vector<std::string>, const std::string&&);
};

#endif
