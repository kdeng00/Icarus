#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <sstream>
#include <cstdlib>

#include <cpr/cpr.h>
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
