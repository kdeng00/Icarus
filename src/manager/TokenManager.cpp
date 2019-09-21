#include "manager/TokenManager.h"

#include <iostream>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <string>
#include <string_view>
#include <sstream>
#include <cstdlib>

#include "manager/DirectoryManager.h"

namespace fs = std::filesystem;


manager::TokenManager::TokenManager()
{
}


model::LoginResult manager::TokenManager::retrieveToken(const model::BinaryPath& bConf)
{
    auto cred = parseAuthCredentials(bConf);

    auto reqObj = createTokenBody(cred);

    std::string uri{cred.uri};
    uri.append("/");
    uri.append(cred.endpoint);

    auto r = cpr::Post(cpr::Url{uri},
        cpr::Body{reqObj.dump()},
        cpr::Header{{"Content-Type", "application/json"},
        {"Connection", "keep-alive"}});


    auto postRes = nlohmann::json::parse(r.text);

    model::LoginResult lr;
    lr.accessToken = postRes["access_token"].get<std::string>();
    lr.tokenType = postRes["token_type"].get<std::string>();
    lr.expiration = postRes["expires_in"].get<int>();

    return lr;
}


bool manager::TokenManager::isTokenValid(std::string& auth, type::Scope scope)
{
    auto authPair = fetchAuthHeader(auth);

    if (!std::get<0>(authPair)) {
        std::cout << "no Bearer found" << std::endl;

        return std::get<0>(authPair);
    }

    auto authHeader = std::get<1>(authPair);

    auto token = authHeader.at(authHeader.size()-1);

    auto scopes = extractScopes(jwt::decode(token));

    switch (scope) {
        case type::Scope::upload:
            return tokenSupportsScope(scopes, "upload:songs");
            break;
        case type::Scope::download:
            return tokenSupportsScope(scopes, "download:songs");
        default:
            break;
    }

    return false;
}

bool manager::TokenManager::testAuth(const model::BinaryPath& bConf)
{
    auto cred = parseAuthCredentials(bConf);
    auto reqObj = createTokenBody(cred);

    std::string uri(cred.uri);
    uri.append("/");
    uri.append(cred.endpoint);

    auto response = sendRequest(uri, reqObj);

    return (response.status_code == 200) ? true : false;
}


cpr::Response manager::TokenManager::sendRequest(std::string_view uri, nlohmann::json& obj)
{
    auto resp = cpr::Post(cpr::Url{uri}, cpr::Body{obj.dump()},
                          cpr::Header{{"Content-type", "application/json"}, 
                                      {"Connection", "keep-alive"}});

    return resp;
}


nlohmann::json manager::TokenManager::createTokenBody(const model::AuthCredentials& auth)
{
    nlohmann::json obj;
    obj["client_id"] = auth.clientId;
    obj["client_secret"] = auth.clientSecret;
    obj["audience"] = auth.apiIdentifier;
    obj["grant_type"] = "client_credentials";

    return obj;
}


model::AuthCredentials manager::TokenManager::parseAuthCredentials(std::string_view path)
{
    auto exe_path = manager::DirectoryManager::configPath(path);
    exe_path.append("/authcredentials.json");
    
    auto con = manager::DirectoryManager::credentialConfigContent(exe_path);

    model::AuthCredentials auth;
    auth.uri = "https://";
    auth.uri.append(con["domain"]);
    auth.apiIdentifier = con["api_identifier"];
    auth.clientId = con["client_id"];
    auth.clientSecret = con["client_secret"];
    auth.endpoint = "oauth/token";

    return auth;
}
model::AuthCredentials manager::TokenManager::parseAuthCredentials(const model::BinaryPath& bConf)
{
    auto con = manager::DirectoryManager::credentialConfigContent(bConf);

    model::AuthCredentials auth;
    auth.uri = "https://";
    auth.uri.append(con["domain"]);
    auth.apiIdentifier = con["api_identifier"];
    auth.clientId = con["client_id"];
    auth.clientSecret = con["client_secret"];
    auth.endpoint = "oauth/token";

    return auth;
}

std::vector<std::string> manager::TokenManager::extractScopes(const jwt::decoded_jwt&& decoded)
{
    std::vector<std::string> scopes;

    for (auto d : decoded.get_payload_claims()) {
        if (d.first.compare("scope") == 0) {
            std::cout << "found scope" << std::endl;
            std::string allScopes(d.second.to_json().get<std::string>());
            std::istringstream iss(allScopes);

            scopes.assign(std::istream_iterator<std::string>(iss), 
                    std::istream_iterator<std::string>());
        }
    }

    return scopes;
}

std::pair<bool, std::vector<std::string>> manager::TokenManager::fetchAuthHeader(const std::string& auth)
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


bool manager::TokenManager::tokenSupportsScope(const std::vector<std::string> scopes, const std::string&& scope)
{
    return std::any_of(scopes.begin(), scopes.end(), 
        [&](std::string foundScope) {
            return (foundScope.compare(scope) == 0);
        });
}
