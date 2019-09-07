#ifndef GENREDTO_H_
#define GENREDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class GenreDto : public oatpp::data::mapping::type::Object
    {
        DTO_INIT(GenreDto, Object)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, category);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
