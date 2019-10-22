#ifndef SONGDTO_H_
#define SONGDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include "model/Models.h"

namespace dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class SongDto : public oatpp::data::mapping::type::Object
    {
        DTO_INIT(SongDto, Object)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, title);
        DTO_FIELD(String, artist);
        DTO_FIELD(String, album);
        DTO_FIELD(String, genre);
        DTO_FIELD(Int32, track);
        DTO_FIELD(Int32, disc);
        DTO_FIELD(Int32, year);
        DTO_FIELD(Int32, duration);
        DTO_FIELD(Int32, coverart_id);
    };

    #include OATPP_CODEGEN_END(DTO)


}

#endif
