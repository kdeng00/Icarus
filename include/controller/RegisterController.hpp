#ifndef REGISTERCONTROLLER_H_
#define REGISTERCONTROLLER_H_

#include <iostream>
#include <memory>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "dto/LoginResultDto.hpp"
#include "dto/conversion/DtoConversions.h"
#include "manager/UserManager.h"
#include "model/Models.h"

using namespace dto;

namespace controller
{
    class RegisterController : public oatpp::web::server::api::ApiController
    {
    public:
        RegisterController(const model::BinaryPath& bConf, 
                           OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) : 
                               oatpp::web::server::api::ApiController(objectMapper),
                               m_bConf(bConf) 
        {
        }

        #include OATPP_CODEGEN_BEGIN(ApiController)

        ENDPOINT("POST", "api/v1/register", registerUser, 
                 BODY_DTO(oatpp::Object<UserDto>, usr))
        {
            manager::UserManager usrMgr(m_bConf);
            auto user = conversion::DtoConversions::toUser(usr);

            OATPP_LOGI("icarus", "Dto converted");

            if (usrMgr.doesUserExist(user)) {
                return createResponse(Status::CODE_401, "user already exists");
            }

            auto res = usrMgr.registerUser(user);
            auto registerResult = dto::conversion::DtoConversions::toRegisterResultDto(res);

            return createDtoResponse(Status::CODE_200, registerResult);
        }
        #include OATPP_CODEGEN_END(ApiController)
    private:
        model::BinaryPath m_bConf;
    };
}

#endif
