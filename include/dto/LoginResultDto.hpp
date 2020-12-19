#ifndef LOGINRESULTDTO_H_
#define LOGINRESULTDTO_H_

#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/Types.hpp>

#include "model/Models.h"

namespace dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class LoginResultDto : public oatpp::DTO
    {
        DTO_INIT(LoginResultDto, DTO)

        DTO_FIELD(oatpp::Int32, id);
        DTO_FIELD(oatpp::String, username);
        DTO_FIELD(oatpp::String, token);
        DTO_FIELD(oatpp::String, token_type);
        DTO_FIELD(oatpp::Int32, expiration);
        DTO_FIELD(oatpp::String, message);
    };

    class RegisterResultDto : public oatpp::DTO
    {
        DTO_INIT(RegisterResultDto, DTO)

        DTO_FIELD(oatpp::String, username);
        DTO_FIELD(oatpp::Boolean, registered);
        DTO_FIELD(oatpp::String, message);
    };

    class UserDto : public oatpp::DTO
    {
        DTO_INIT(UserDto, DTO)

        DTO_FIELD(oatpp::Int32, userId);
        DTO_FIELD(oatpp::String, firstname);
        DTO_FIELD(oatpp::String, lastname);
        DTO_FIELD(oatpp::String, phone);
        DTO_FIELD(oatpp::String, email);
        DTO_FIELD(oatpp::String, username);
        DTO_FIELD(oatpp::String, password);
    };

    #include OATPP_CODEGEN_END(DTO)

}

#endif
