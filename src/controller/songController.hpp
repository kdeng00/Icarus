#ifndef SONGCONTROLLER_H_
#define SONGCONTROLLER_H_

#include <iostream>
#include <filesystem>
#include <string>
#include <memory>
#include <vector>

#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/mime/multipart/InMemoryPartReader.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "managers/song_manager.h"
#include "models.h"
#include "token_manager.h"
#include "types/scopes.h"

class songController : public oatpp::web::server::api::ApiController
{
public:
    songController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), exe_path(p)
    { }

    #include OATPP_CODEGEN_BEGIN(ApiController)

    // TODO: work on uploading a song
    // and clean up
    ENDPOINT("POST", "/api/v1/song/data", songUpload, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        auto authHeader = request->getHeader("Authorization");

        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

        auto auth = authHeader->std_str();
        
        //std::cout << "auth " << auth << std::endl;
       
        token_manager tok;
        if (!tok.is_token_valid(auth, Scope::upload)) {
            // TODO: prevent user from moving forward
            // token did not have the specified scope (permission)
        }

        auto mp = std::make_shared<oatpp::web::mime::multipart::Multipart>(request->getHeaders());

        oatpp::web::mime::multipart::Reader mp_reader(mp.get());

        mp_reader.setPartReader("file", oatpp::web::mime::multipart::createInMemoryPartReader(dataSize));

        request->transferBody(&mp_reader);

        auto file = mp->getNamedPart("file");

        OATPP_ASSERT_HTTP(file, Status::CODE_400, "file is null");

        auto stream = file->getInputStream();
        auto buff = std::unique_ptr<char>(new char[file->getKnownSize()]);
        auto buffSize = stream->read(buff.get(), file->getKnownSize());

        std::vector<unsigned char> data(buff.get(), buff.get() + buffSize);

        Song sng;
        sng.data = std::move(data);
        
        song_manager s_mgr(exe_path);
        s_mgr.saveSong(sng);

        return createResponse(Status::CODE_200, "OK");
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
    std::string exe_path;

    const long dataSize = 1000000000000;
};

#endif
