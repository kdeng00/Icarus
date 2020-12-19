#ifndef DTOCONVERSIONS_H_
#define DTOCONVERSIONS_H_

#include <iostream>
#include <chrono>
#include <thread>

#include <oatpp/core/data/mapping/ObjectMapper.hpp>
#include <oatpp/core/data/mapping/type/Type.hpp>
#include <oatpp/core/data/mapping/type/Object.hpp>
#include <oatpp/core/Types.hpp>

#include "dto/AlbumDto.hpp"
#include "dto/ArtistDto.hpp"
#include "dto/CoverArtDto.hpp"
#include "dto/GenreDto.hpp"
#include "dto/LoginResultDto.hpp"
#include "dto/SongDto.hpp"
#include "dto/YearDto.hpp"
#include "model/Models.h"

using namespace model;
using namespace dto;

namespace dto::conversion
{
    class DtoConversions
    {
    public:
        template<typename D = oatpp::Object<LoginResultDto>>
        static D toLoginResultDto(const User &user, const Token &token)
        {
            auto logRes = LoginResultDto::createShared();
            logRes->id = (user.id != 0) ? user.id : 0;
            logRes->username = (!user.username.empty()) ? user.username.c_str() : "None";
            logRes->token = (!token.accessToken.empty()) ? token.accessToken.c_str() : "None";
            logRes->token_type = (!token.tokenType.empty()) ? token.tokenType.c_str() : "None";
            logRes->expiration = (token.expiration != 0) ? token.expiration : 0;

            return logRes;
        }

        template<typename D = oatpp::Object<RegisterResultDto>>
        static D toRegisterResultDto(
            const model::RegisterResult &regRes)
        {
            auto result = RegisterResultDto::createShared();
            result->message = (!regRes.message.empty()) ? regRes.message.c_str() : "None";
            result->registered = regRes.registered;
            result->username = (!regRes.username.empty()) ? regRes.username.c_str() : "None";

            return result;
        }

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

        template<typename D = oatpp::Object<ArtistDto>>
        static D toArtistDto(const Artist &artist)
        {
            auto result = ArtistDto::createShared();
            result->id = (artist.id != 0) ? artist.id : 0;
            result->artist = (!artist.artist.empty()) ? artist.artist.c_str() : "None";

            return result;
        }


        template<typename D = oatpp::Object<CoverArtDto>>
        static D toCoverDto(const Cover &cover)
        {
            auto result = CoverArtDto::createShared();
            result->id = cover.id != 0 ? cover.id : 0;
            result->songTitle = (!cover.songTitle.empty()) ? cover.songTitle.c_str() : "None";

            return result;
        }

        template<typename D = oatpp::Object<GenreDto>>
        static D toGenreDto(const Genre &genre)
        {
            auto result = GenreDto::createShared();
            result->id = (genre.id != 0) ? genre.id : 0;
            result->category = (!genre.category.empty()) ? genre.category.c_str() : "None";

            return result;
        }


        template<typename D = oatpp::Object<SongDto>>
        static D toSongDto(const model::Song &song)
        {
            auto result = SongDto::createShared();
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

            return result;
        }

        template<typename D = oatpp::Object<YearDto>>
        static D toYearDto(const Year &year)
        {
            auto result = YearDto::createShared();
            result->id = (year.id != 0) ? year.id : 0;
            result->year = (year.year != 0) ? year.year : 0;

            return result;
        }


        template<typename D = oatpp::Object<SongDto>>
        static Song toSong(const D &songDto)
        {
            Song song;
            song.id = (songDto->id.getPtr() == nullptr) ? 0 : *songDto->id;
            song.title = (songDto->title == nullptr) ? "" : songDto->title->c_str();
            song.album = (songDto->album == nullptr) ? "" : songDto->album->c_str();
            song.artist = (songDto->artist == nullptr) ? "" : songDto->artist->c_str();
            song.albumArtist = (songDto->album_artist == nullptr) ? 
                "" : songDto->album_artist->c_str();
            song.genre = (songDto->genre == nullptr) ? "" : songDto->genre->c_str();
            song.year = (songDto->year.getPtr() == nullptr) ? 0 : *songDto->year;
            song.track = (songDto->track.getPtr() == nullptr) ? 0 : *songDto->track;
            song.disc = (songDto->disc.getPtr() == nullptr) ? 0 : *songDto->disc;
            song.coverArtId = (songDto->coverart_id.getPtr() == nullptr) ? 
                0 : *songDto->coverart_id;

        return song;

        }

        template<typename D = oatpp::Object<UserDto>>
        static User toUser(const D &userDto)
        {
            User user;
            user.id = (userDto->userId.getPtr()) ? 0 : *userDto->userId;
            user.firstname = (userDto->firstname == nullptr) ? "" : userDto->firstname->c_str();
            user.lastname = (userDto->lastname == nullptr) ? "" : userDto->lastname->c_str();
            user.phone = (userDto->phone == nullptr) ? "" : userDto->phone->c_str();
            user.email = (userDto->email == nullptr) ? "" : userDto->email->c_str();
            user.username = (userDto->username == nullptr) ? "" : userDto->username->c_str();
            user.password = (userDto->password == nullptr) ? "" : userDto->password->c_str();


            std::cout << "Over\n";

            return user;
        }
    };
}

#endif
