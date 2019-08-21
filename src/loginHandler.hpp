#ifndef LOGINHANDLER_H_
#define LOGINHANDLER_H_

#include <memory>

#include "oatpp/network/server/Server.hpp"
#include "oatpp/network/server/SimpleTCPConnectionProvider.hpp"
#include "oatpp/web/server/HttpConnectionHandler.hpp"

#include "dto/loginResultDto.hpp"

class loginHandler : public oatpp::web::server::HttpRequestHandler
{
public:

    std::shared_ptr<OutgoingResponse> handle(const std::shared_ptr<IncomingRequest>& request) override
    {
        auto logRes = loginResultDto::createShared();
        logRes->access_token = "hahahahahahaha";
        logRes->token_type = "Fly";

        return ResponseFactory::createResponse(Status::CODE_200, "ddd");
    }
private:
    OATPP_COMPONENT(std::shared_ptr<oatpp::data::mapping::ObjectMapper>, m_objectMapper);
};

#endif
