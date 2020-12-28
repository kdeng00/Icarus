#ifndef BASECONTROLLER_H_
#define BASECONTROLLER_H_

#include <memory>

#include "icarus_lib/icarus.h"
// #include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"


namespace controller
{
    class BaseController : public oatpp::web::server::api::ApiController
    {
        protected:
            BaseController(const icarus_lib::binary_path &bConf, 
                OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper)) :
                    oatpp::web::server::api::ApiController(object_mapper),
                    m_bConf(bConf)
            {
            }

            icarus_lib::binary_path m_bConf;
        private:
    };
}


#endif