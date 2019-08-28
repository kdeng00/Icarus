#ifndef SONGDTO_H_
#define SONGDTO_H_

#include "oatpp/core/data/mapping/type/Object.hpp"
#include "oatpp/core/macro/codegen.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class songDto : public oatpp::data::mapping::type::Object
{
    DTO_INIT(songDto, Object)

    DTO_FIELD(Int32, id);
    DTO_FIELD(String, title);
    DTO_FIELD(String, artist);
    DTO_FIELD(String, album);
    DTO_FIELD(String, genre);
    DTO_FIELD(Int32, year);
    DTO_FIELD(Int32, duration);
};

#include OATPP_CODEGEN_END(DTO)

#endif