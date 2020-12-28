#ifndef TOKEN_MANAGER_H_
#define TOKEN_MANAGER_H_

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <cpr/cpr.h>
#include "icarus_lib/icarus.h"
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>

#include "model/Models.h"
#include "type/Scopes.h"

namespace manager {
    class TokenManager {
    public:
        TokenManager() = default;

        model::Token retrieveToken(const icarus_lib::binary_path &);

        bool isTokenValid(std::string&, type::Scope);
        bool testAuth(const icarus_lib::binary_path &);
    private:
        cpr::Response sendRequest(std::string_view, nlohmann::json&);

        nlohmann::json createTokenBody(const model::AuthCredentials&);

        model::AuthCredentials parseAuthCredentials(const icarus_lib::binary_path &);

        std::vector<std::string> extractScopes(const jwt::decoded_jwt&&);
        std::pair<bool, std::vector<std::string>> fetchAuthHeader(const std::string&);

        bool tokenSupportsScope(const std::vector<std::string>&, const std::string&&);
    };
}

#endif
