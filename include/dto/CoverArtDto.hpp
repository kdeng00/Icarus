#ifndef COVERARTDTO_H_
#define COVERARTDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace dto {
    #include OATPP_CODEGEN_BEGIN(DTO)

    class CoverArtDto : public oatpp::data::mapping::type::Object {
        DTO_INIT(CoverArtDto, Object)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, songTitle);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
