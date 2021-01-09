#ifndef TOKEN_MANAGER_H_
#define TOKEN_MANAGER_H_

#include <string>
#include <sstream>
#include <fstream>
#include <string_view>
#include <utility>
#include <vector>
#include <chrono>
#include <algorithm>
#include <cstdlib>

#include <cpr/cpr.h>
#include <icarus_lib/icarus.h>
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>

#include "manager/BaseManager.hpp"
#include "manager/DirectoryManager.h"
#include "database/Repositories.hpp"
#include "type/Scopes.h"

namespace manager
{
    template<typename token_val>
    class TokenManager : public BaseManager<icarus_lib::binary_path>
    {
    public:
        TokenManager() = default;
        TokenManager(icarus_lib::binary_path &config) : BaseManager(config)
        {
        }

        template <typename config_path = icarus_lib::binary_path>
        [[deprecated("This function has been deprecated")]]
        token_val retrieveToken(const config_path &bConf)
        {
            auto cred = parseAuthCredentials(bConf);
            auto reqObj = createTokenBody<nlohmann::json>(cred);

            std::stringstream uri_builder;
            uri_builder << cred.uri << "/" << cred.endpoint;
            auto uri = uri_builder.str();

            auto r = sendRequest(uri, reqObj);
            auto postRes = nlohmann::json::parse(r.text);

            token_val lr(postRes["access_token"],
                    postRes["token_type"],
                    postRes["expires_in"]);

            return lr;
        }

        template <typename config_path = icarus_lib::binary_path, typename user = icarus_lib::user>
        token_val create_token(const config_path &config, const user& usr)
        {
            std::cout << "Fetching icarus key config\n";
            auto t = DirectoryManager::keyConfigContent(config);

            std::string private_key_path(t["rsa_private_key_path"]);
            std::string public_key_path(t["rsa_public_key_path"]);

            auto private_key = DirectoryManager::contentOfPath(private_key_path);
            auto public_key = DirectoryManager::contentOfPath(public_key_path);

            auto current_time = std::chrono::system_clock::now();

            auto scopes = all_scopes<std::string_view>();
            auto ss = all_scopes_spaced<std::string_view>(scopes);

            token_val token;
            token.issued = current_time;
            token.expires = current_time + std::chrono::hours(hours_till_expire());

            auto tok = jwt::create()
                .set_issuer("icarus")
                .set_type("JWS")
                .set_issued_at(token.issued)
                .set_expires_at(token.expires)
                .set_payload_claim("scope", jwt::claim(ss))
                .set_payload_claim("username", jwt::claim(usr.username))
                .sign(jwt::algorithm::rs256(public_key, private_key, "", ""));

            token.access_token = tok;

            database::token_rep repo(config);
            repo.create_token(token, usr);


            return token;
        }



        template<typename scope = type::Scope, template <typename, typename> class token_result = std::pair>
        auto is_token_valid(token_val &tokn, scope scp)
        {
            token_result<bool, std::string> result;

            auto decoded = is_token_verified(tokn);

            if (!decoded.first)
            {
                result.first = false;
                result.second = "Invalid";

                return result;
            }

            for (auto &payload : decoded.second.get_payload_claims())
            {
                if (payload.first.compare("iat") == 0)
                {
                    auto issued_val = payload.second;
                }
                else if (payload.first.compare("exp") == 0)
                {
                    auto expired_val = payload.second;
                }
                else if (payload.first.compare("scope") == 0)
                {
                }
            }

            result.first = true;
            result.second = "Valid token";

            return result;
        }

        bool isTokenValid(std::string &auth, type::Scope scope)
        {
            auto authPair = fetchAuthHeader(auth);

            if (!std::get<0>(authPair)) {
                std::cout << "no Bearer found\n";

                return std::get<0>(authPair);
            }

            auto authHeader = std::get<1>(authPair);

            auto token = authHeader.at(authHeader.size()-1);

            auto scopes = extractScopes(jwt::decode(token));

            switch (scope)
            {
            case type::Scope::upload:
                return tokenSupportsScope(scopes, "upload:songs");
            case type::Scope::download:
                return tokenSupportsScope(scopes, "download:songs");
            case type::Scope::stream:
                return tokenSupportsScope(scopes, "stream:songs");
            case type::Scope::deleteSong:
                return tokenSupportsScope(scopes, "delete:songs");
            case type::Scope::updateSong:
                return tokenSupportsScope(scopes, "update:songs");
            case type::Scope::retrieveSong:
                return tokenSupportsScope(scopes, "read:song_details");
            case type::Scope::retrieveAlbum:
                return tokenSupportsScope(scopes, "read:albums");
            case type::Scope::retrieveArtist:
                return tokenSupportsScope(scopes, "read:artists");
            case type::Scope::retrieveGenre:
                return tokenSupportsScope(scopes, "read:genre");
            case type::Scope::retrieveYear:
                return tokenSupportsScope(scopes, "read:year");
            case type::Scope::downloadCoverArt:
                return tokenSupportsScope(scopes, "download:cover_art");
            default:
                break;
            }

            return false;
        }

        bool testAuth(const icarus_lib::binary_path &bConf)
        {
            auto cred = parseAuthCredentials(bConf);
            auto reqObj = createTokenBody<nlohmann::json>(cred);

            std::stringstream uri_builder;
            uri_builder << cred.uri << "/" << cred.endpoint;
            auto uri = uri_builder.str();

            auto response = sendRequest(uri, reqObj);

            return (response.status_code == 200) ? true : false;
        }
    private:
        cpr::Response sendRequest(std::string_view uri, nlohmann::json &obj)
        {
            const std::string uriString(uri.begin(), uri.end());

            auto resp = cpr::Post(cpr::Url(uriString), cpr::Body{obj.dump()},
                              cpr::Header{{"Content-type", "application/json"}, 
                                          {"Connection", "keep-alive"}});

            return resp;
        }


        template<typename json,
                 typename auth_cred = icarus_lib::auth_credentials>
        json createTokenBody(const auth_cred &auth)
        {
            json obj;
            obj["client_id"] = auth.client_id;
            obj["client_secret"] = auth.client_secret;
            obj["audience"] = auth.api_identifier;
            obj["grant_type"] = "client_credentials";

            return obj;
        }


        template<typename config_path = icarus_lib::binary_path, typename auth_cred = icarus_lib::auth_credentials>
        auth_cred parseAuthCredentials(const config_path &bConf)
        {
            auto con = DirectoryManager::credentialConfigContent(bConf);

            auth_cred auth;
            auth.uri = "https://";
            auth.uri.append(con["domain"]);
            auth.api_identifier = con["api_identifier"];
            auth.client_id = con["client_id"];
            auth.client_secret = con["client_secret"];
            auth.endpoint = "oauth/token";

            return auth;
        }


        template<typename str_type = std::string, typename container = std::vector<str_type>,
                 typename jwt_decoded = jwt::decoded_jwt>
        container extractScopes(jwt_decoded &&decoded)
        {
            container scopes;
            auto payload_claims = decoded.get_payload_claims();
            scopes.reserve(payload_claims.size());

            for (auto &d : payload_claims)
            {
                if (d.first.compare("scope") == 0) {
                    std::cout << "found scope\n";
                    auto scopes_js = d.second.to_json();
                    auto scs = scopes_js.template get<str_type>();
                    str_type allScopes(scs);
                    std::istringstream iss(allScopes);

                    scopes.assign(std::istream_iterator<str_type>(iss), std::istream_iterator<str_type>());
                }
            }

            return scopes;
        }


        template<typename str_type = std::string, typename container = std::vector<str_type>,
                 typename pair = std::pair<bool, container>>
        pair fetchAuthHeader(const str_type &auth)
        {
            std::istringstream iss(auth);
            container authHeader{std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>()};

            bool foundBearer = false;

            auto lamb = std::any_of(authHeader.begin(), authHeader.end(), [&](std::string_view word)
                { 
                    return (word.compare("Bearer") == 0); 
                });

            if (lamb)
            {
                std::cout << "Bearer found\n";
                foundBearer = true;
            }

            return std::make_pair(foundBearer, authHeader);
        }


        auto is_token_verified(const token_val &tokn)
        {
            auto t = DirectoryManager::keyConfigContent(m_config);

            std::string private_key_path(t["rsa_private_key_path"]);
            std::string public_key_path(t["rsa_public_key_path"]);

            auto private_key = DirectoryManager::contentOfPath(private_key_path);
            auto public_key = DirectoryManager::contentOfPath(public_key_path);

            auto verify = jwt::verify()
                .allow_algorithm(jwt::algorithm::rs256(public_key, private_key, "", ""
                            ))
                .with_issuer("icarus");
            auto decoded = jwt::decode(tokn.access_token);

            try
            {
                verify.verify(decoded);
            }
            catch (jwt::signature_verification_exception &e)
            {
                auto s = e;
                std::cout << "Something happend: " << e.what() << "\n";
                return std::make_pair(false, jwt::decoded_jwt(tokn.access_token));
            }

            return std::make_pair(true, decoded);
        }


        template<typename val_t, typename array_t = std::array<val_t, 11>>
        constexpr array_t all_scopes() noexcept
        {
            constexpr array_t scopes{"upload:songs",
                                     "download:songs",
                                     "stream:songs",
                                     "delete:songs",
                                     "update:songs",
                                     "read:song_details",
                                     "read:albums",
                                     "read:artists",
                                     "read:genre",
                                     "read:year",
                                     "download:cover_art"};

            return scopes;
        }

        template<typename val_t, typename array_t = std::array<val_t, 11>>
        auto all_scopes_spaced(array_t &scopes)
        {
            std::stringstream ss;
            ss << scopes[0];

            std::for_each(scopes.begin() + 1, scopes.end(), [&ss](val_t v)
                    {
                        ss << " " << v;
                    });

            return ss.str();
        }


        constexpr auto hours_till_expire() noexcept
        {
            return 4;
        }

        // Hours in which a refreshed token will expire
        constexpr auto refresh_token_expiration() noexcept
        {
            return 24;
        }


        template<typename str_val = std::string, typename container = std::vector<str_val>>
        auto tokenSupportsScope(const container &scopes, str_val &&scope)
        {
            return std::any_of(scopes.begin(), scopes.end(), [&scope](std::string_view foundScope)
                {
                    return (foundScope.compare(scope) == 0);
                });
        }
    };
}

#endif
