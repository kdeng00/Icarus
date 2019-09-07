#ifndef YEARDTO_H_
#define YEARDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class YearDto : public oatpp::data::mapping::type::Object
    {
        DTO_INIT(YearDto, Object)

        DTO_FIELD(Int32, id);
        DTO_FIELD(Int32, year);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
