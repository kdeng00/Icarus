#ifndef SONGDTO_H_
#define SONGDTO_H_

#include <oatpp/core/Types.hpp>
#include <oatpp/core/macro/codegen.hpp>

namespace dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class SongDto : public oatpp::DTO
    {
        DTO_INIT(SongDto, DTO)

        DTO_FIELD(oatpp::Int32, id);
        DTO_FIELD(oatpp::String, title);
        DTO_FIELD(oatpp::String, artist);
        DTO_FIELD(oatpp::String, album_artist);
        DTO_FIELD(oatpp::String, album);
        DTO_FIELD(oatpp::String, genre);
        DTO_FIELD(oatpp::Int32, track);
        DTO_FIELD(oatpp::Int32, disc);
        DTO_FIELD(oatpp::Int32, year);
        DTO_FIELD(oatpp::Int32, duration);
        DTO_FIELD(oatpp::Int32, coverart_id);
    };

    #include OATPP_CODEGEN_END(DTO)


}

#endif
