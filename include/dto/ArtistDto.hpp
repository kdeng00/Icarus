#ifndef ARTISTDTO_H_
#define ARTISTDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

namespace dto {
    #include OATPP_CODEGEN_BEGIN(DTO)

    class ArtistDto : public oatpp::data::mapping::type::Object {
        DTO_INIT(ArtistDto, Object)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, artist);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
