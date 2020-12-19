#ifndef ALBUMDTO_H_
#define ALBUMDTO_H_

#include <oatpp/core/Types.hpp>
#include <oatpp/core/macro/codegen.hpp>

namespace dto
{
    #include OATPP_CODEGEN_BEGIN(DTO)

    class AlbumDto : public oatpp::DTO
    {
        DTO_INIT(AlbumDto, DTO)

        DTO_FIELD(Int32, id);
        DTO_FIELD(String, title);
        DTO_FIELD(String, artist);
        DTO_FIELD(Int32, year);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
