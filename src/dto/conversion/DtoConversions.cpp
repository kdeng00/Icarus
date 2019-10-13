#include "dto/conversion/DtoConversions.h"

namespace dto { namespace conversion { 
    dto::LoginResultDto::ObjectWrapper DtoConversions::toLoginResultDto(const model::User& user, 
        const model::Token& token) {
        auto logRes = dto::LoginResultDto::createShared();
        logRes->username = user.username.c_str();
        logRes->token = token.accessToken.c_str();
        logRes->token_type = token.tokenType.c_str();
        logRes->expiration = token.expiration;

        return logRes;
    }


    dto::RegisterResultDto::ObjectWrapper DtoConversions::toRegisterResultDto(
        const model::RegisterResult& regRes) {
        auto result = dto::RegisterResultDto::createShared();
        result->message = regRes.message.c_str();
        result->registered = regRes.registered;
        result->username = regRes.username.c_str();

        return result;
    }


    model::Song DtoConversions::toSong(dto::SongDto::ObjectWrapper& songDto) {
        model::Song song;
        song.id = (songDto->id.getPtr() == nullptr) ? 0 : songDto->id->getValue();
        song.title = (songDto->title == nullptr) ? "" : songDto->title->c_str();
        song.album = (songDto->album == nullptr) ? "" : songDto->album->c_str();
        song.artist = (songDto->artist == nullptr) ? "" : songDto->artist->c_str();
        song.genre = (songDto->genre == nullptr) ? "" : songDto->genre->c_str();
        song.year = (songDto->year.getPtr() == nullptr) ? 0 : songDto->year->getValue();
        song.track = (songDto->track.getPtr() == nullptr) ? 0 : songDto->track->getValue();
        song.disc = (songDto->disc.getPtr() == nullptr) ? 0 : songDto->disc->getValue();

        return song;
    }


    model::User DtoConversions::toUser(dto::UserDto::ObjectWrapper& userDto) {
        model::User user;
        user.id = (userDto->userId.getPtr() == nullptr) ? 0 : userDto->userId->getValue();
        user.firstname = (userDto->firstname == nullptr) ? "" : userDto->firstname->c_str();
        user.lastname = (userDto->lastname == nullptr) ? "" : userDto->lastname->c_str();
        user.phone = (userDto->phone == nullptr) ? "" : userDto->phone->c_str();
        user.email = (userDto->email == nullptr) ? "" : userDto->email->c_str();
        user.username = (userDto->username == nullptr) ? "" : userDto->username->c_str();
        user.password = (userDto->password == nullptr) ? "" : userDto->password->c_str();

        return user;
    }
}}
