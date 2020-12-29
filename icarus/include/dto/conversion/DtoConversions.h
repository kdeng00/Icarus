#ifndef DTOCONVERSIONS_H_
#define DTOCONVERSIONS_H_

#include <iostream>
#include <chrono>
#include <thread>

#include "icarus_lib/icarus.h"
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

using namespace dto;

namespace dto::conversion
{
    class DtoConversions
    {
    public:
        template<typename D = oatpp::Object<LoginResultDto>>
        static D toLoginResultDto(const icarus_lib::user &user, 
            const icarus_lib::token &token)
        {
            auto logRes = LoginResultDto::createShared();
            logRes->id = (user.id != 0) ? user.id : 0;
            logRes->username = (!user.username.empty()) ? user.username.c_str() : "None";
            logRes->token = (!token.access_token.empty()) ? token.access_token.c_str() : "None";
            logRes->token_type = (!token.token_type.empty()) ? token.token_type.c_str() : "None";
            logRes->expiration = (token.expiration != 0) ? token.expiration : 0;

            return logRes;
        }

        template<typename D = oatpp::Object<RegisterResultDto>>
        static D toRegisterResultDto(
            const icarus_lib::register_result &regRes)
        {
            auto result = RegisterResultDto::createShared();
            result->message = (!regRes.message.empty()) ? regRes.message.c_str() : "None";
            result->registered = regRes.registered;
            result->username = (!regRes.username.empty()) ? regRes.username.c_str() : "None";

            return result;
        }

        template<typename D = oatpp::Object<AlbumDto>>
        static D toAlbumDto(const icarus_lib::album &album)
        {
            auto result = AlbumDto::createShared();
            result->id = (album.id != 0) ? album.id : 0;
            result->title = (!album.title.empty()) ? album.title.c_str() : "";
            result->artist = (!album.artist.empty()) ? album.artist.c_str() : "";
            result->year = (album.year != 0) ? album.year : 0;

            return result;
        }

        template<typename D = oatpp::Object<ArtistDto>>
        static D toArtistDto(const icarus_lib::artist &artist)
        {
            auto result = ArtistDto::createShared();
            result->id = (artist.id != 0) ? artist.id : 0;
            result->artist = (!artist.name.empty()) ? artist.name.c_str() : "None";

            return result;
        }


        template<typename D = oatpp::Object<CoverArtDto>>
        static D toCoverDto(const icarus_lib::cover &cover)
        {
            auto result = CoverArtDto::createShared();
            result->id = cover.id != 0 ? cover.id : 0;
            result->songTitle = (!cover.song_title.empty()) ? cover.song_title.c_str() : "None";

            return result;
        }

        template<typename D = oatpp::Object<GenreDto>>
        static D toGenreDto(const icarus_lib::genre &genre)
        {
            auto result = GenreDto::createShared();
            result->id = (genre.id != 0) ? genre.id : 0;
            result->category = (!genre.category.empty()) ? genre.category.c_str() : "None";

            return result;
        }


        template<typename D = oatpp::Object<SongDto>>
        static D toSongDto(const icarus_lib::song &song)
        {
            auto result = SongDto::createShared();
            result->id = (song.id != 0) ? song.id : 0;
            result->title = (!song.title.empty()) ? song.title.c_str() : "";
            result->album = (!song.album.empty()) ? song.album.c_str() : "";
            result->artist = (!song.artist.empty()) ? song.artist.c_str() : "";
            result->album_artist = (!song.album_artist.empty()) ? song.album_artist.c_str() : "";
            result->genre = (!song.genre.empty()) ? song.genre.c_str() : "";
            result->duration = (song.duration != 0) ? song.duration : 0;
            result->year = (song.year != 0) ? song.year : 0;
            result->track = (song.track != 0) ? song.track : 0;
            result->disc = (song.disc != 0) ? song.disc : 0;
            result->coverart_id = (song.cover_art_id != 0) ? song.cover_art_id : 0;

            return result;
        }

        template<typename D = oatpp::Object<YearDto>>
        static D toYearDto(const icarus_lib::year &year)
        {
            auto result = YearDto::createShared();
            result->id = (year.id != 0) ? year.id : 0;
            result->year = (year.song_year != 0) ? year.song_year : 0;

            return result;
        }


        template<typename D = oatpp::Object<SongDto>>
        static icarus_lib::song toSong(const D &songDto)
        {
            icarus_lib::song song;
            song.id = (songDto->id.getPtr() == nullptr) ? 0 : *songDto->id;
            song.title = (songDto->title == nullptr) ? "" : songDto->title->c_str();
            song.album = (songDto->album == nullptr) ? "" : songDto->album->c_str();
            song.artist = (songDto->artist == nullptr) ? "" : songDto->artist->c_str();
            song.album_artist = (songDto->album_artist == nullptr) ? 
                "" : songDto->album_artist->c_str();
            song.genre = (songDto->genre == nullptr) ? "" : songDto->genre->c_str();
            song.year = (songDto->year.getPtr() == nullptr) ? 0 : *songDto->year;
            song.track = (songDto->track.getPtr() == nullptr) ? 0 : *songDto->track;
            song.disc = (songDto->disc.getPtr() == nullptr) ? 0 : *songDto->disc;
            song.cover_art_id = (songDto->coverart_id.getPtr() == nullptr) ? 
                0 : *songDto->coverart_id;

        return song;

        }

        template<typename D = oatpp::Object<UserDto>>
        static icarus_lib::user toUser(const D &userDto)
        {
            icarus_lib::user user;
            user.id = (userDto->userId.getPtr()) ? 0 : *userDto->userId;
            user.firstname = (userDto->firstname == nullptr) ? "" : userDto->firstname->c_str();
            user.lastname = (userDto->lastname == nullptr) ? "" : userDto->lastname->c_str();
            user.phone = (userDto->phone == nullptr) ? "" : userDto->phone->c_str();
            user.email = (userDto->email == nullptr) ? "" : userDto->email->c_str();
            user.username = (userDto->username == nullptr) ? "" : userDto->username->c_str();
            user.password = (userDto->password == nullptr) ? "" : userDto->password->c_str();


            return user;
        }
    };
}

#endif
