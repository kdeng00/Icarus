#ifndef LOGINCONTROLLER_H_
#define LOGINCONTROLLER_H_

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "../dto/loginResultDto.hpp"

class loginController : public oatpp::web::server::api::ApiController
{
public:
    loginController(OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper)
    { }

    #include OATPP_CODEGEN_BEGIN(ApiController)
    
    ENDPOINT("POST", "/api/v1/login", root)
    {
        OATPP_LOGI("icarus", "logging in");

        auto logRes = loginResultDto::createShared();
        logRes->access_token = "sdfsdfsdfsdert";
        logRes->token_type = "Special";

        return createDtoResponse(Status::CODE_200, logRes);
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
};

#endif
