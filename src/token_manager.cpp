#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>
#include <cstdlib>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

//#include "models.h"
#include "token_manager.h"

namespace fs = std::filesystem;

//extern "C"
//{


token_manager::token_manager()
{
}

loginResult token_manager::retrieve_token()
{
    auto cred = parse_auth_credentials();

    loginResult lr;
    lr.access_token = "dsfdsf";
    lr.token_type = "demo";

    return lr;
}

auth_credentials token_manager::parse_auth_credentials()
{
    auth_credentials auth;
    //auto ss = working_path.string();
    //auto path = fs::canonical(fs::path("."));
    //std::cout << "canonical path " << path.string() << std::endl;

    return auth;
}

/**
LoginRes* token_manager::retrieve_token(TokenReq *tok)
{
    LoginRes *res = new LoginRes;

    nlohmann::json reqObj;
    reqObj["client_id"] = tok->ClientId;
    reqObj["client_secret"] = tok->ClientSecret;
    reqObj["audience"] = tok->Audience;
    reqObj["grant_type"] = tok->GrantType;

    std::string uri{tok->URI};
    uri.append("/");
    uri.append(tok->Endpoint);

    auto r = cpr::Post(cpr::Url{uri},
        cpr::Body{reqObj.dump()},
        cpr::Header{{"Content-Type", "application/json"}});

    auto post_res = nlohmann::json::parse(r.text);
    strcpy(res->Token, post_res["access_token"].get<std::string>().c_str());
    strcpy(res->TokenType, post_res["token_type"].get<std::string>().c_str());
    res->Expiration = post_res["expires_in"].get<int>();
    strcpy(res->Message, "Success");

    return res;
}
*/
//}
