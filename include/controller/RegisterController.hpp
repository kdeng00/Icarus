#ifndef REGISTERCONTROLLER_H_
#define REGISTERCONTROLLER_H_

#include <iostream>
#include <memory>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "dto/LoginResultDto.hpp"
#include "manager/UserManager.h"
#include "model/Models.h"

namespace controller
{
    class RegisterController : public oatpp::web::server::api::ApiController
    {
    public:
        RegisterController(const model::BinaryPath& bConf, 
                            OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) :
            oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf) { }

        #include OATPP_CODEGEN_BEGIN(ApiController)

        ENDPOINT("POST", "api/v1/register", registerUser, BODY_DTO(dto::UserDto::ObjectWrapper, usr)) {
            manager::UserManager usrMgr(m_bConf);
            auto user = usrMgr.userDtoConv(usr);

            usrMgr.registerUser(user);

            return createResponse(Status::CODE_200, "Reg");
        }
        #include OATPP_CODEGEN_END(ApiController)
    private:
        model::BinaryPath m_bConf;
    };
}

#endif
