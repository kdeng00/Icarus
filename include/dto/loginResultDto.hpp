#ifndef LOGINRESULTDTO_H_
#define LOGINRESULTDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace Dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class loginResultDto : public oatpp::data::mapping::type::Object
    {
        DTO_INIT(loginResultDto, Object)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, username);
        DTO_FIELD(String, token);
        DTO_FIELD(String, token_type);
        DTO_FIELD(Int32, expiration);
        DTO_FIELD(String, message);
    };

    class userDto : public oatpp::data::mapping::type::Object
    {
        DTO_INIT(userDto, Object)

        DTO_FIELD(String, username);
        DTO_FIELD(String, password);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
