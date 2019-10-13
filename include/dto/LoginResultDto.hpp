#ifndef LOGINRESULTDTO_H_
#define LOGINRESULTDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include "model/Models.h"

namespace dto {
    #include OATPP_CODEGEN_BEGIN(DTO)

    class LoginResultDto : public oatpp::data::mapping::type::Object {
        DTO_INIT(LoginResultDto, Object)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, username);
        DTO_FIELD(String, token);
        DTO_FIELD(String, token_type);
        DTO_FIELD(Int32, expiration);
        DTO_FIELD(String, message);
    };

    class RegisterResultDto : public oatpp::data::mapping::type::Object {
        DTO_INIT(RegisterResultDto, Object)

        DTO_FIELD(String, username);
        DTO_FIELD(Boolean, registered);
        DTO_FIELD(String, message);
    };

    class UserDto : public oatpp::data::mapping::type::Object {
        DTO_INIT(UserDto, Object)

        DTO_FIELD(Int32, userId);
        DTO_FIELD(String, firstname);
        DTO_FIELD(String, lastname);
        DTO_FIELD(String, phone);
        DTO_FIELD(String, email);
        DTO_FIELD(String, username);
        DTO_FIELD(String, password);
    };

    #include OATPP_CODEGEN_END(DTO)

}

#endif
