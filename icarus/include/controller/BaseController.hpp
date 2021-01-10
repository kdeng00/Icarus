#ifndef BASECONTROLLER_H_
#define BASECONTROLLER_H_

#include <memory>

#include <icarus_lib/icarus.h>
#include <oatpp/core/macro/codegen.hpp>
#include <oatpp/core/macro/component.hpp>
#include <oatpp/core/Types.hpp>
#include <oatpp/web/server/api/ApiController.hpp>
#include <oatpp/web/server/handler/AuthorizationHandler.hpp>

using oatpp::web::server::api::ApiController;
using oatpp::web::server::handler::BearerAuthorizationHandler;


namespace controller
{
    class BaseController : public ApiController
    {
        protected:
            BaseController(const icarus_lib::binary_path &bConf, 
                OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper)) :
                    ApiController(object_mapper),
                    m_bConf(bConf)
            {
                setDefaultAuthorizationHandler(std::make_shared<BearerAuthorizationHandler>("Authorization"));
            }

            constexpr auto no_auth_header_key()
            {
                return "No Authorization key provided";
            }

            icarus_lib::binary_path m_bConf;
        private:
    };
}


#endif
