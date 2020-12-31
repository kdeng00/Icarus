#ifndef TOKEN_MANAGER_H_
#define TOKEN_MANAGER_H_

#include <string>
#include <sstream>
#include <fstream>
#include <string_view>
#include <utility>
#include <vector>
#include <chrono>
#include <cstdlib>

#include <cpr/cpr.h>
#include <icarus_lib/icarus.h>
#include <jwt-cpp/jwt.h>
#include <nlohmann/json.hpp>

#include "manager/DirectoryManager.h"
#include "type/Scopes.h"

namespace manager
{
    // template<typename token_val = icarus_lib::token>
    class TokenManager
    {
    public:
        TokenManager() = default;

        icarus_lib::token retrieveToken(const icarus_lib::binary_path &bConf)
        {
	    	auto cred = parseAuthCredentials(bConf);
    		auto reqObj = createTokenBody(cred);

		    std::string uri{cred.uri};
	    	uri.append("/");
    		uri.append(cred.endpoint);

		    auto r = sendRequest(uri, reqObj);
	    	auto postRes = nlohmann::json::parse(r.text);

    		icarus_lib::token lr(std::move(postRes["access_token"].get<std::string>()),
			    	std::move(postRes["token_type"].get<std::string>()), 
				    postRes["expires_in"].get<int>());

		    return lr;
        }

        template <typename token_t = icarus_lib::token, typename config_path = icarus_lib::binary_path>
        token_t create_token(const config_path &config)
        {
            std::cout << "Fetching icarus key config\n";
            auto t = DirectoryManager::keyConfigContent(config);
            // std::cout << t.dump(4) << "\n";

            std::string private_key_path(t["rsa_private_key_path"]);
            std::string public_key_path(t["rsa_public_key_path"]);

            // std::cout << "prv key path " << private_key_path << "\n";
            // std::cout << "pub key path " << public_key_path << "\n";

            auto private_key = DirectoryManager::contentOfPath(private_key_path);
            auto public_key = DirectoryManager::contentOfPath(public_key_path);

            // std::cout << private_key << "\n";
            // std::cout << public_key << "\n";

            auto current_time = std::chrono::system_clock::now();
            constexpr auto hours_expire = 4;

            // TODO: Continue to work on this.

            auto tok = jwt::create()
                .set_issuer("icarus")
                .set_type("JWS")
                .set_issued_at(current_time)
                .set_expires_at(current_time + std::chrono::hours(hours_expire))
                .sign(jwt::algorithm::rs256(public_key, private_key, "", ""));

            token_t token;
            token.access_token = tok;

            return token;
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
    		auto reqObj = createTokenBody(cred);

	    	std::string uri(cred.uri);
		    uri.append("/");
    		uri.append(cred.endpoint);

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


        nlohmann::json createTokenBody(const icarus_lib::auth_credentials &auth)
        {
		    nlohmann::json obj;
    		obj["client_id"] = auth.client_id;
	    	obj["client_secret"] = auth.client_secret;
		    obj["audience"] = auth.api_identifier;
    		obj["grant_type"] = "client_credentials";

	    	return obj;
        }


        icarus_lib::auth_credentials parseAuthCredentials(const icarus_lib::binary_path &bConf)
        {
		    auto con = DirectoryManager::credentialConfigContent(bConf);

    		icarus_lib::auth_credentials auth;
	    	auth.uri = "https://";
		    auth.uri.append(con["domain"]);
    		auth.api_identifier = con["api_identifier"];
	    	auth.client_id = con["client_id"];
		    auth.client_secret = con["client_secret"];
    		auth.endpoint = "oauth/token";

	    	return auth;
        }


        std::vector<std::string> extractScopes(jwt::decoded_jwt &&decoded)
        {
		    std::vector<std::string> scopes;
            auto payload_claims = decoded.get_payload_claims();
            scopes.reserve(payload_claims.size());

    		for (auto &d : payload_claims)
            {
	    	    if (d.first.compare("scope") == 0) {
		            std::cout << "found scope\n";
		            std::string allScopes(d.second.to_json().get<std::string>());
		            std::istringstream iss(allScopes);

    		        scopes.assign(std::istream_iterator<std::string>(iss), 
	    	                std::istream_iterator<std::string>());
		        }
		    }

    		return scopes;
        }


        std::pair<bool, std::vector<std::string>> fetchAuthHeader(const std::string &auth)
        {
		    std::istringstream iss(auth);
    		std::vector<std::string> authHeader{std::istream_iterator<std::string>(iss),
	    			std::istream_iterator<std::string>()};

    		bool foundBearer = false;

	    	if (std::any_of(authHeader.begin(), authHeader.end(), 
		           [&](std::string_view word)
                   { 
		               return (word.compare("Bearer") == 0); 
                   }))
            {
                std::cout << "Bearer found\n";
                foundBearer = true;
		    }

		    return std::make_pair(foundBearer, authHeader);
        }


        bool tokenSupportsScope(const std::vector<std::string> &scopes, std::string &&scope)
        {
		    return std::any_of(scopes.begin(), scopes.end(), 
		                       [&](std::string_view foundScope)
                               {
		                           return (foundScope.compare(scope) == 0);
		                       });
        }
    };
}

#endif
