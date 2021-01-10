#include "manager/TokenManager.h"

using std::stringstream;
using std::string;
using std::string_view;
using std::cout;
using std::chrono::system_clock;
using std::chrono::hours;


using icarus_lib::binary_path;
using icarus_lib::token;
using icarus_lib::user;

using type::Scope;


namespace manager
{
    TokenManager::TokenManager(binary_path &config) : BaseManager(config)
    {
    }

    token TokenManager::create_token(const binary_path &config, const user& usr)
    {
        cout << "Fetching icarus key config\n";
        auto t = DirectoryManager::keyConfigContent(config);

        string private_key_path(t["rsa_private_key_path"]);
        string public_key_path(t["rsa_public_key_path"]);

        auto private_key = DirectoryManager::contentOfPath(private_key_path);
        auto public_key = DirectoryManager::contentOfPath(public_key_path);

        auto current_time = system_clock::now();

        auto scopes = all_scopes<string_view>();
        auto ss = all_scopes_spaced<string_view>(scopes);

        token token;
        token.issued = current_time;
        token.expires = current_time + hours(hours_till_expire());

        auto tok = jwt::create()
            .set_issuer("icarus")
            .set_type("JWS")
            .set_issued_at(token.issued)
            .set_expires_at(token.expires)
            .set_payload_claim("scope", jwt::claim(ss))
            .set_payload_claim("username", jwt::claim(usr.username))
            .sign(jwt::algorithm::rs256(public_key, private_key, "", ""));

        token.access_token = tok;

        database::token_repo repo(config);
        repo.create_token(token, usr);


        return token;
    }



    bool TokenManager::isTokenValid(string &auth, Scope scope)
    {
        auto authPair = fetchAuthHeader(auth);

        if (!std::get<0>(authPair)) {
            cout << "no Bearer found\n";

            return std::get<0>(authPair);
        }

        auto authHeader = std::get<1>(authPair);

        auto token = authHeader.at(authHeader.size()-1);

        auto scopes = extractScopes(jwt::decode(token));

        switch (scope)
        {
        case Scope::upload:
            return tokenSupportsScope(scopes, "upload:songs");
        case Scope::download:
            return tokenSupportsScope(scopes, "download:songs");
        case Scope::stream:
            return tokenSupportsScope(scopes, "stream:songs");
        case Scope::deleteSong:
            return tokenSupportsScope(scopes, "delete:songs");
        case Scope::updateSong:
            return tokenSupportsScope(scopes, "update:songs");
        case Scope::retrieveSong:
            return tokenSupportsScope(scopes, "read:song_details");
        case Scope::retrieveAlbum:
            return tokenSupportsScope(scopes, "read:albums");
        case Scope::retrieveArtist:
            return tokenSupportsScope(scopes, "read:artists");
        case Scope::retrieveGenre:
            return tokenSupportsScope(scopes, "read:genre");
        case Scope::retrieveYear:
            return tokenSupportsScope(scopes, "read:year");
        case Scope::downloadCoverArt:
            return tokenSupportsScope(scopes, "download:cover_art");
        default:
            break;
        }

        return false;
    }
}
