#ifndef GENREDTO_H_
#define GENREDTO_H_

#include "oatpp/core/macro/codegen.hpp"
#include <oatpp/core/Types.hpp>

namespace dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class GenreDto : public oatpp::DTO
    {
        DTO_INIT(GenreDto, DTO)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, category);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
