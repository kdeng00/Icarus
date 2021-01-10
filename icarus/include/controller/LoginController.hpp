#ifndef LOGINCONTROLLER_H_
#define LOGINCONTROLLER_H_

#include <iostream>
#include <string>

#include <icarus_lib/icarus.h>

#include "controller/BaseController.hpp"
#include "dto/conversion/DtoConversions.h"
#include "dto/LoginResultDto.hpp"
#include "dto/conversion/DtoConversions.h"
#include "manager/Manager.h"
#include "manager/UserManager.hpp"

using icarus_lib::binary_path;

namespace controller
{
    class LoginController : public BaseController
    {
    public:
        LoginController(const icarus_lib::binary_path &bConf, 
                        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper)) : 
                            BaseController(bConf, object_mapper)
        {
        }


        #include OATPP_CODEGEN_BEGIN(ApiController)
    
        ENDPOINT("POST", "/api/v1/login", data, BODY_DTO(oatpp::Object<UserDto>, usr))
        {
            OATPP_LOGI("icarus", "logging in");

            manager::UserManager<icarus_lib::user> usrMgr(m_bConf);
            auto user = dto::conversion::DtoConversions::toUser(usr);

            if (!usrMgr.doesUserExist(user) || !usrMgr.validatePassword(user)) {
                auto logRes = dto::LoginResultDto::createShared();

                logRes->message = "invalid credentials";

                std::cout << "user does not exist\n";

                return createDtoResponse(Status::CODE_401, logRes);
            }

            database::UserRepository usr_repo(m_bConf);
            user = usr_repo.retrieveUserRecord(user);

            manager::token_manager tok;
            auto token = tok.create_token(m_bConf, user);
            // auto token = tok.retrieveToken(m_bConf);

            auto logRes = dto::conversion::DtoConversions::toLoginResultDto(user, token);
            logRes->message = "Successful";

            return createDtoResponse(Status::CODE_200, logRes);
        }

        #include OATPP_CODEGEN_END(ApiController)
    private:
    };
}
#endif
