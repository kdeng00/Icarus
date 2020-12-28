#include "manager/TokenManager.h"

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <string_view>
#include <sstream>
#include <cstdlib>

#include "manager/DirectoryManager.h"

namespace manager {
    model::Token TokenManager::retrieveToken(const icarus_lib::binary_path & bConf) {
		auto cred = parseAuthCredentials(bConf);
		auto reqObj = createTokenBody(cred);

		std::string uri{cred.uri};
		uri.append("/");
		uri.append(cred.endpoint);

		auto r = sendRequest(uri, reqObj);
		auto postRes = nlohmann::json::parse(r.text);

		model::Token lr(std::move(postRes["access_token"].get<std::string>()),
				std::move(postRes["token_type"].get<std::string>()), 
				postRes["expires_in"].get<int>());

		return lr;
    }


    bool TokenManager::isTokenValid(std::string& auth, type::Scope scope) {
		auto authPair = fetchAuthHeader(auth);

		if (!std::get<0>(authPair)) {
		    std::cout << "no Bearer found\n";

		    return std::get<0>(authPair);
		}

		auto authHeader = std::get<1>(authPair);

		auto token = authHeader.at(authHeader.size()-1);

		auto scopes = extractScopes(jwt::decode(token));

		switch (scope) {
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

    bool TokenManager::testAuth(const icarus_lib::binary_path & bConf) {
		auto cred = parseAuthCredentials(bConf);
		auto reqObj = createTokenBody(cred);

		std::string uri(cred.uri);
		uri.append("/");
		uri.append(cred.endpoint);

		auto response = sendRequest(uri, reqObj);

		return (response.status_code == 200) ? true : false;
    }


    cpr::Response TokenManager::sendRequest(std::string_view uri, nlohmann::json& obj) {
		const std::string uriString(uri.begin(), uri.end());

		auto resp = cpr::Post(cpr::Url(uriString), cpr::Body{obj.dump()},
                          cpr::Header{{"Content-type", "application/json"}, 
                                      {"Connection", "keep-alive"}});

		return resp;
    }


    nlohmann::json TokenManager::createTokenBody(const model::AuthCredentials& auth) {
		nlohmann::json obj;
		obj["client_id"] = auth.clientId;
		obj["client_secret"] = auth.clientSecret;
		obj["audience"] = auth.apiIdentifier;
		obj["grant_type"] = "client_credentials";

		return obj;
    }


    model::AuthCredentials TokenManager::parseAuthCredentials(const icarus_lib::binary_path & bConf) {
		auto con = DirectoryManager::credentialConfigContent(bConf);

		model::AuthCredentials auth;
		auth.uri = "https://";
		auth.uri.append(con["domain"]);
		auth.apiIdentifier = con["api_identifier"];
		auth.clientId = con["client_id"];
		auth.clientSecret = con["client_secret"];
		auth.endpoint = "oauth/token";

		return auth;
    }


    std::vector<std::string> TokenManager::extractScopes(const jwt::decoded_jwt&& decoded) {
		std::vector<std::string> scopes;

		for (auto& d : decoded.get_payload_claims()) {
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

    std::pair<bool, std::vector<std::string>> TokenManager::fetchAuthHeader(const std::string& auth) {
		std::istringstream iss(auth);
		std::vector<std::string> authHeader{std::istream_iterator<std::string>(iss),
				std::istream_iterator<std::string>()
		};

		bool foundBearer = false;
		if (std::any_of(authHeader.begin(), authHeader.end(), 
		       [&](std::string_view word) { 
		           return (word.compare("Bearer") == 0); })) {
		    std::cout << "Bearer found\n";
		    foundBearer = true;
		}

		return std::make_pair(foundBearer, authHeader);
    }


    bool TokenManager::tokenSupportsScope(const std::vector<std::string>& scopes, 
		const std::string&& scope) {
		return std::any_of(scopes.begin(), scopes.end(), 
		    [&](std::string_view foundScope) {
		        return (foundScope.compare(scope) == 0);
		    });
    }
}
