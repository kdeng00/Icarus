#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <cstdlib>

#include <cpr/cpr.h>
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>

#include "token_manager.h"

namespace fs = std::filesystem;


token_manager::token_manager()
{
}

loginResult token_manager::retrieve_token()
{
    loginResult lr;
    lr.access_token = "dsfdsf";
    lr.token_type = "demo";

    return lr;
}
loginResult token_manager::retrieve_token(std::string_view path)
{
    auto cred = parse_auth_credentials(path);

    nlohmann::json reqObj;
    reqObj["client_id"] = cred.client_id;
    reqObj["client_secret"] = cred.client_secret;
    reqObj["audience"] = cred.api_identifier;
    reqObj["grant_type"] = "client_credentials";

    std::string uri{cred.uri};
    uri.append("/");
    uri.append(cred.endpoint);

    auto r = cpr::Post(cpr::Url{uri},
        cpr::Body{reqObj.dump()},
        cpr::Header{{"Content-Type", "application/json"},
        {"Connection", "keep-alive"}});

    auto post_res = nlohmann::json::parse(r.text);

    loginResult lr;
    lr.access_token = post_res["access_token"].get<std::string>();
    lr.token_type = post_res["token_type"].get<std::string>();
    lr.expiration = post_res["expires_in"].get<int>();

    return lr;
}

bool token_manager::is_token_valid(std::string& auth, Scope scope)
{
    std::istringstream iss(auth);
    std::vector<std::string> authHeader{std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>()
    };

    auto wordCount = 0;
    /**
    for (auto& word : authHeader) {
        std::cout << "word " << wordCount++ << " " << word << std::endl;
    }
    */

    if (!std::any_of(authHeader.begin(), authHeader.end(), [](std::string word)
                { 
                std::cout << "comparing " << word << " to Bearer" << std::endl;
                return (word.compare("Bearer") == 0);
                })) {
        std::cout << "Bearer not found" << std::endl;
        return false;
    }

    auto token = authHeader.at(authHeader.size()-1);
    std::cout << "going to decode " << token << std::endl;
    auto decoded = jwt::decode(token);

    std::vector<std::string> scopes;
    for (auto d : decoded.get_payload_claims()) {
        if (d.first.compare("scope") == 0) {
            std::cout << "found scope" << std::endl;
            std::string all_scopes(d.second.to_json().get<std::string>());
            std::istringstream iss(all_scopes);

            scopes.assign(std::istream_iterator<std::string>(iss), 
                    std::istream_iterator<std::string>());
            //std::cout << scopes << std::endl;
        }
    }

    std::cout << "printing all scopes" << std::endl;

    for (auto& scope_obj : scopes) {
        std::cout << scope_obj << std::endl;
    }
    

    std::cout << "goodbye" << std::endl;
    exit(1);

    switch (scope) {
        case Scope::upload:
            break;
        default:
            break;
    }

    return true;
}

auth_credentials token_manager::parse_auth_credentials(std::string_view path)
{
    auto exe_path = fs::canonical(path).parent_path().string();
    exe_path.append("/authcredentials.json");
    
    std::fstream a(exe_path, std::ios::in);
    std::stringstream s;
    s << a.rdbuf();
    a.close();

    auto con = nlohmann::json::parse(s.str());

    auth_credentials auth;
    auth.uri = "https://";
    auth.uri.append(con["Domain"]);
    auth.api_identifier = con["ApiIdentifier"];
    auth.client_id = con["ClientId"];
    auth.client_secret = con["ClientSecret"];
    auth.endpoint = "oauth/token";

    return auth;
}
