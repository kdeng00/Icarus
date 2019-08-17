#ifndef LOGINRESULTDTO_H_
#define LOGINRESULTDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class loginResultDto : public oatpp::data::mapping::type::Object
{
    DTO_INIT(loginResultDto, Object)

    DTO_FIELD(String, access_token);
    DTO_FIELD(String, token_type);
};

#include OATPP_CODEGEN_END(DTO)

#endif
