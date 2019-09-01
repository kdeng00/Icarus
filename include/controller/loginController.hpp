#ifndef LOGINCONTROLLER_H_
#define LOGINCONTROLLER_H_

#include <iostream>
#include <string>
#include <memory>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "dto/loginResultDto.hpp"
#include "managers/token_manager.h"

namespace Controller
{
    class loginController : public oatpp::web::server::api::ApiController
    {
    public:
        loginController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper), exe_path(p)
        { }
        loginController(const Model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            :oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf)
        { }


        #include OATPP_CODEGEN_BEGIN(ApiController)
    
        ENDPOINT("POST", "/api/v1/login", data, BODY_DTO(Dto::userDto::ObjectWrapper, usr)) {
            OATPP_LOGI("icarus", "logging in");

            Manager::token_manager tok;
            //auto token = tok.retrieve_token(exe_path);
            auto token = tok.retrieve_token(m_bConf);

            auto logRes = Dto::loginResultDto::createShared();
            logRes->id = 0; // TODO: change this later on to something meaningful
            logRes->username = usr->username->c_str();
            logRes->token = token.access_token.c_str();
            logRes->token_type = token.token_type.c_str();
            logRes->expiration = token.expiration;
            logRes->message = "Successful";

            return createDtoResponse(Status::CODE_200, logRes);
        }

        #include OATPP_CODEGEN_END(ApiController)
    private:
        std::string exe_path;
        Model::BinaryPath m_bConf;
    };
}
#endif
