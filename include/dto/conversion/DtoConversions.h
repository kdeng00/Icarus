#ifndef DTOCONVERSIONS_H_
#define DTOCONVERSIONS_H_

#include <oatpp/core/data/mapping/type/Type.hpp>
#include <oatpp/core/data/mapping/type/Object.hpp>
#include <oatpp/core/Types.hpp>

#include "dto/AlbumDto.hpp"
#include "dto/LoginResultDto.hpp"
#include "dto/SongDto.hpp"
#include "model/Models.h"
#include "oatpp/core/data/mapping/ObjectMapper.hpp"

using model::User;
using model::Song;
using model::Token;
using model::RegisterResult;

using namespace model;
using namespace dto;

namespace dto { namespace conversion {
    class DtoConversions {
    public:
        // static dto::LoginResultDto::ObjectWrapper toLoginResultDto(const model::User&, const model::Token&);
        // static oatpp::data::mapping::type::ObjectWrapper<dto::LoginResultDto> toLoginResultDto(const model::User &user, const model::Token &token)
        // static dto::LoginResultDto::ObjectWrapper toLoginResultDto(const model::User &user, const model::Token &token)
        // static dto::LoginResultDto::ObjectWrapper toLoginResultDto(const User &user, const Token &token)
        template<typename D = LoginResultDto>
        static D toLoginResultDto(const User &user, const Token &token)
        {
            // const model::Token& token) {
            auto logRes = LoginResultDto::createShared();
            /**
            logRes->username = user.username.c_str();
            logRes->token = token.accessToken.c_str();
            logRes->token_type = token.tokenType.c_str();
            logRes->expiration = token.expiration;
            */

            return logRes;
        }

        // static dto::RegisterResultDto::ObjectWrapper toRegisterResultDto(
        template<typename D = RegisterResultDto>
        static D toRegisterResultDto(
            const model::RegisterResult &regRes)
        {
            auto result = RegisterResultDto::createShared();
            /**
            result->message = regRes.message.c_str();
            result->registered = regRes.registered;
            result->username = regRes.username.c_str();
            */

            return result;
        }

        // static dto::AlbumDto::ObjectWrapper toAlbumDto(const model::Album&);
        template<typename D = oatpp::Object<AlbumDto>>
        static D toAlbumDto(const Album &album)
        {
            auto result = AlbumDto::createShared();
            result->id = (album.id != 0) ? album.id : 0;
            result->title = (!album.title.empty()) ? album.title.c_str() : "";
            result->artist = (!album.artist.empty()) ? album.artist.c_str() : "";
            result->year = (album.year != 0) ? album.year : 0;

            return result;
        }

        template<typename D = SongDto>
        static D toSongDto(const model::Song &song)
        {
            auto result = SongDto::createShared();
            /**
            result->id = (song.id != 0) ? song.id : 0;
            result->title = (!song.title.empty()) ? song.title.c_str() : "";
            result->album = (!song.album.empty()) ? song.album.c_str() : "";
            result->artist = (!song.artist.empty()) ? song.artist.c_str() : "";
            result->album_artist = (!song.albumArtist.empty()) ? song.albumArtist.c_str() : "";
            result->genre = (!song.genre.empty()) ? song.genre.c_str() : "";
            result->duration = (song.duration != 0) ? song.duration : 0;
            result->year = (song.year != 0) ? song.year : 0;
            result->track = (song.track != 0) ? song.track : 0;
            result->disc = (song.disc != 0) ? song.disc : 0;
            result->coverart_id = (song.coverArtId != 0) ? song.coverArtId : 0;
            */

            return result;
        }

        // static model::Song toSong(dto::SongDto::ObjectWrapper&);
        template<typename D = oatpp::Object<SongDto>, typename Song = Song>
        static model::Song toSong(D songDto)
        {
            Song song;
            /**
            song.id = (songDto.id.getPtr() == nullptr) ? 0 : songDto.id->getValue();
            song.title = (songDto.title == nullptr) ? "" : songDto.title.c_str();
            song.album = (songDto.album == nullptr) ? "" : songDto.album.c_str();
            song.artist = (songDto.artist == nullptr) ? "" : songDto.artist.c_str();
            song.albumArtist = (songDto.album_artist == nullptr) ? 
                "" : songDto.album_artist.c_str();
            song.genre = (songDto.genre == nullptr) ? "" : songDto.genre.c_str();
            song.year = (songDto.year.getPtr() == nullptr) ? 0 : songDto.year.getValue();
            song.track = (songDto.track.getPtr() == nullptr) ? 0 : songDto.track.getValue();
            song.disc = (songDto.disc.getPtr() == nullptr) ? 0 : songDto.disc.getValue();
            song.coverArtId = (songDto.coverart_id.getPtr() == nullptr) ? 
                0 : songDto.coverart_id.getValue();
                */

        return song;

        }

        // static model::User toUser(dto::UserDto::ObjectWrapper&);
        template<typename D = UserDto>
        static User toUser(D &userDto)
        {
            model::User user;
            /**
            user.id = (userDto->userId.getPtr() == nullptr) ? 0 : userDto->userId->getValue();
            user.firstname = (userDto->firstname == nullptr) ? "" : userDto->firstname->c_str();
            user.lastname = (userDto->lastname == nullptr) ? "" : userDto->lastname->c_str();
            user.phone = (userDto->phone == nullptr) ? "" : userDto->phone->c_str();
            user.email = (userDto->email == nullptr) ? "" : userDto->email->c_str();
            user.username = (userDto->username == nullptr) ? "" : userDto->username->c_str();
            user.password = (userDto->password == nullptr) ? "" : userDto->password->c_str();
            */

            return user;
        }
    };
}}

#endif
