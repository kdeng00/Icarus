#ifndef COVERARTDTO_H_
#define COVERARTDTO_H_

#include "oatpp/core/macro/codegen.hpp"
#include <oatpp/core/Types.hpp>

namespace dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class CoverArtDto : public oatpp::DTO
    {
        DTO_INIT(CoverArtDto, DTO)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, songTitle);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
