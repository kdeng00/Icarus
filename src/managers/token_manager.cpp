#include "managers/token_manager.h"

#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <sstream>
#include <cstdlib>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

#include "managers/directory_manager.h"

namespace fs = std::filesystem;


Manager::token_manager::token_manager()
{
}

Model::loginResult Manager::token_manager::retrieve_token()
{
    Model::loginResult lr;
    lr.access_token = "dsfdsf";
    lr.token_type = "demo";

    return lr;
}
Model::loginResult Manager::token_manager::retrieve_token(std::string_view path)
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

    Model::loginResult lr;
    lr.access_token = post_res["access_token"].get<std::string>();
    lr.token_type = post_res["token_type"].get<std::string>();
    lr.expiration = post_res["expires_in"].get<int>();

    return lr;
}
Model::loginResult Manager::token_manager::retrieve_token(const Model::BinaryPath& bConf)
{
    auto cred = parse_auth_credentials(bConf);

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

    Model::loginResult lr;
    lr.access_token = post_res["access_token"].get<std::string>();
    lr.token_type = post_res["token_type"].get<std::string>();
    lr.expiration = post_res["expires_in"].get<int>();

    return lr;
}

bool Manager::token_manager::is_token_valid(std::string& auth, Scope scope)
{
    auto authPair = fetch_auth_header(auth);

    if (!std::get<0>(authPair)) {
        std::cout << "no Bearer found" << std::endl;

        return std::get<0>(authPair);
    }

    auto authHeader = std::get<1>(authPair);

    auto token = authHeader.at(authHeader.size()-1);

    auto scopes = extract_scopes(jwt::decode(token));

    switch (scope) {
        case Scope::upload:
            return token_supports_scope(scopes, "upload:songs");
            break;
        case Scope::download:
            return token_supports_scope(scopes, "download:songs");
        default:
            break;
    }

    return false;
}

Model::auth_credentials Manager::token_manager::parse_auth_credentials(std::string_view path)
{
    auto exe_path = Manager::directory_manager::configPath(path);
    exe_path.append("/authcredentials.json");
    
    auto con = Manager::directory_manager::credentialConfigContent(exe_path);

    Model::auth_credentials auth;
    auth.uri = "https://";
    auth.uri.append(con["domain"]);
    auth.api_identifier = con["api_identifier"];
    auth.client_id = con["client_id"];
    auth.client_secret = con["client_secret"];
    auth.endpoint = "oauth/token";

    return auth;
}
Model::auth_credentials Manager::token_manager::parse_auth_credentials(const Model::BinaryPath& bConf)
{
    auto exePath = Manager::directory_manager::configPath(bConf);
    exePath.append("/auth_credentials.json");

    auto con = Manager::directory_manager::credentialConfigContent(exePath);

    Model::auth_credentials auth;
    auth.uri = "https://";
    auth.uri.append(con["domain"]);
    auth.api_identifier = con["api_identifier"];
    auth.client_id = con["client_id"];
    auth.client_secret = con["client_secret"];
    auth.endpoint = "oauth/token";

    return auth;
}

std::vector<std::string> Manager::token_manager::extract_scopes(const jwt::decoded_jwt&& decoded)
{
    std::vector<std::string> scopes;

    for (auto d : decoded.get_payload_claims()) {
        if (d.first.compare("scope") == 0) {
            std::cout << "found scope" << std::endl;
            std::string all_scopes(d.second.to_json().get<std::string>());
            std::istringstream iss(all_scopes);

            scopes.assign(std::istream_iterator<std::string>(iss), 
                    std::istream_iterator<std::string>());
        }
    }

    return scopes;
}

std::pair<bool, std::vector<std::string>> Manager::token_manager::fetch_auth_header(const std::string& auth)
{
    std::istringstream iss(auth);
    std::vector<std::string> authHeader{std::istream_iterator<std::string>(iss),
        std::istream_iterator<std::string>()
    };

    bool foundBearer = false;
    if (std::any_of(authHeader.begin(), authHeader.end(), 
           [](std::string word) { 
               return (word.compare("Bearer") == 0);
           })) {
        std::cout << "Bearer found" << std::endl;
        foundBearer = true;
    }

    return std::make_pair(foundBearer, authHeader);
}

bool Manager::token_manager::token_supports_scope(const std::vector<std::string> scopes, const std::string&& scope)
{
    return std::any_of(scopes.begin(), scopes.end(), 
        [&](std::string foundScope) {
            return (foundScope.compare(scope) == 0);
        });
}
