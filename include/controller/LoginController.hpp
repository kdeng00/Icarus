#ifndef LOGINCONTROLLER_H_
#define LOGINCONTROLLER_H_

#include <iostream>
#include <string>
#include <memory>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "dto/LoginResultDto.hpp"
#include "manager/TokenManager.h"
#include "model/Models.h"

namespace controller
{
    class LoginController : public oatpp::web::server::api::ApiController
    {
    public:
        LoginController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper), exe_path(p)
        { }
        LoginController(const model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            :oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf)
        { }


        #include OATPP_CODEGEN_BEGIN(ApiController)
    
        ENDPOINT("POST", "/api/v1/login", data, BODY_DTO(dto::UserDto::ObjectWrapper, usr)) {
            OATPP_LOGI("icarus", "logging in");

            manager::TokenManager tok;
            auto token = tok.retrieveToken(m_bConf);

            auto logRes = dto::LoginResultDto::createShared();
            logRes->id = 0; // TODO: change this later on to something meaningful
            logRes->username = usr->username->c_str();
            logRes->token = token.accessToken.c_str();
            logRes->token_type = token.tokenType.c_str();
            logRes->expiration = token.expiration;
            logRes->message = "Successful";

            return createDtoResponse(Status::CODE_200, logRes);
        }

        #include OATPP_CODEGEN_END(ApiController)
    private:
        std::string exe_path;
        model::BinaryPath m_bConf;
    };
}
#endif
