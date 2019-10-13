#ifndef DTOCONVERSIONS_H_
#define DTOCONVERSIONS_H_

#include "dto/LoginResultDto.hpp"
#include "dto/SongDto.hpp"
#include "model/Models.h"

namespace dto { namespace conversion {
    class DtoConversions {
    public:
        static dto::LoginResultDto::ObjectWrapper toLoginResultDto(const model::User&, const model::Token&);

        static dto::RegisterResultDto::ObjectWrapper toRegisterResultDto(
            const model::RegisterResult&);

        static model::Song toSong(dto::SongDto::ObjectWrapper&);

        static model::User toUser(dto::UserDto::ObjectWrapper&);
    };
}}

#endif
