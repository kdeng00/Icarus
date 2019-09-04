#ifndef TOKEN_MANAGER_H_
#define TOKEN_MANAGER_H_

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <jwt-cpp/jwt.h>

#include "model/Models.h"
#include "type/Scopes.h"

namespace manager
{
    class TokenManager
    {
    public:
        TokenManager();

        model::LoginResult retrieveToken();
        model::LoginResult retrieveToken(std::string_view);
        model::LoginResult retrieveToken(const model::BinaryPath&);

        bool isTokenValid(std::string&, type::Scope);
    private:
        model::AuthCredentials parseAuthCredentials(std::string_view);
        model::AuthCredentials parseAuthCredentials(const model::BinaryPath&);

        std::vector<std::string> extractScopes(const jwt::decoded_jwt&&);
        std::pair<bool, std::vector<std::string>> fetchAuthHeader(const std::string&);

        bool tokenSupportsScope(const std::vector<std::string>, const std::string&&);
    };
}

#endif
