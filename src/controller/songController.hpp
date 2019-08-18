#ifndef SONGCONTROLLER_H_
#define SONGCONTROLLER_H_

#include <iostream>
#include <filesystem>
#include <string>
#include <memory>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/mime/multipart/InMemoryPartReader.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

//#include "../dto/DTO.h"

class songController : public oatpp::web::server::api::ApiController
{
public:
    songController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), exe_path(p)
    { }

    #include OATPP_CODEGEN_BEGIN(ApiController)

    // TODO: work on uploading a song
    ENDPOINT("POST", "/api/v1/song/data", songUpload, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
    {

        return createResonse(Status::CODE_200, "OK");
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
    std::string exe_path;
};

#endif
