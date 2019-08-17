#ifndef LOGINCONTROLLER_H_
#define LOGINCONTROLLER_H_

#include <iostream>
#include <filesystem>
#include <string>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "../dto/loginResultDto.hpp"
#include "token_manager.h"

namespace fs = std::filesystem;

class loginController : public oatpp::web::server::api::ApiController
{
public:
    loginController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), exe_path(fs::canonical(p.c_str()).parent_path().string())
    {
        std::cout << "p is " << p << std::endl;
        std::cout << "working path " << exe_path.string() << std::endl;
    }


    #include OATPP_CODEGEN_BEGIN(ApiController)
    
    ENDPOINT("POST", "/api/v1/login", root)
    {
        OATPP_LOGI("icarus", "logging in");

        token_manager tok;
        auto token = tok.retrieve_token();

        auto logRes = loginResultDto::createShared();
        logRes->access_token = token.access_token.c_str();
        logRes->token_type = token.token_type.c_str();

        return createDtoResponse(Status::CODE_200, logRes);
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
    fs::path exe_path;
};

#endif
