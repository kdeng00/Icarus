#ifndef DTOCONVERSIONS_H_
#define DTOCONVERSIONS_H_

#include "dto/AlbumDto.hpp"
#include "dto/LoginResultDto.hpp"
#include "dto/SongDto.hpp"
#include "model/Models.h"

namespace dto { namespace conversion {
    class DtoConversions {
    public:
        // static dto::LoginResultDto::ObjectWrapper toLoginResultDto(const model::User&, const model::Token&);
        static dto::LoginResultDto toLoginResultDto(const model::User&, const model::Token&);

        // static dto::RegisterResultDto::ObjectWrapper toRegisterResultDto(
        static dto::RegisterResultDto toRegisterResultDto(
            const model::RegisterResult&);

        // static dto::AlbumDto::ObjectWrapper toAlbumDto(const model::Album&);
        static dto::AlbumDto toAlbumDto(const model::Album&);

        static dto::SongDto toSongDto(const model::Song&);

        // static model::Song toSong(dto::SongDto::ObjectWrapper&);
        static model::Song toSong(dto::SongDto*);

        // static model::User toUser(dto::UserDto::ObjectWrapper&);
        static model::User toUser(dto::UserDto&);
    };
}}

#endif
