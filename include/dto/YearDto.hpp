#ifndef YEARDTO_H_
#define YEARDTO_H_

#include "oatpp/core/macro/codegen.hpp"
#include <oatpp/core/Types.hpp>

namespace dto {
    #include OATPP_CODEGEN_BEGIN(DTO)

    class YearDto : public oatpp::DTO {
        DTO_INIT(YearDto, DTO)

        DTO_FIELD(Int32, id);
        DTO_FIELD(Int32, year);
    };

    #include OATPP_CODEGEN_END(DTO)
}

#endif
