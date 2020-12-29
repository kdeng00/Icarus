#ifndef REGISTERCONTROLLER_H_
#define REGISTERCONTROLLER_H_

#include <iostream>
#include <memory>

#include "icarus_lib/icarus.h"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/core/Types.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "dto/LoginResultDto.hpp"
#include "dto/conversion/DtoConversions.h"
#include "manager/UserManager.hpp"

using namespace dto;

namespace controller
{
    class RegisterController : public BaseController
    {
    public:
        RegisterController(const icarus_lib::binary_path &bConf, 
                           OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper)) : 
                                BaseController(bConf, object_mapper)
        {
        }

        #include OATPP_CODEGEN_BEGIN(ApiController)

        ENDPOINT("POST", "api/v1/register", registerUser, 
                 BODY_DTO(oatpp::Object<UserDto>, usr))
        {
            manager::UserManager<icarus_lib::user> usrMgr(m_bConf);
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
    };
}

#endif
