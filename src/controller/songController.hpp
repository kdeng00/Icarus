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
        auto mp = std::make_shared<oatpp::web::mime::multipart::Multipart>(request->getHeaders());

        oatpp::web::mime::multipart::Reader mp_reader(mp.get());

        mp_reader.setPartReader("file", oatpp::web::mime::multipart::createInMemoryPartReader(dataSize));

        request->transferBody(&mp_reader);

        auto file = mp->getNamedPart("file");

        OATPP_ASSERT_HTTP(file, Status::CODE_400, "file is null");

        auto stream = file->getInputStream();
        //char *buff = new char[file->getKnownSize()];
        auto buff = std::unique_ptr<char>(new char[file->getKnownSize()]);
        //std::vector<unsigned char> buff; 
        //buff.reserve(file->getKnownSize());
        //std::string buff;
        //std::vector<unsigned char> buff;
        auto buffSize = stream->read(buff.get(), file->getKnownSize()-1);
        //auto buffSize = stream->read(&buff[0], file->getKnownSize()-1);
        std::cout << "buff size " << buffSize << std::endl;
        /**
        std::cout << "buff " << buff << std::endl;
        //std::cout << stream.get() << std::endl;
        */
        std::vector<unsigned char> data(buff.get(), buff.get() + buffSize);
        std::cout << "size of data " << data.size() << std::endl;

        Song sng;
        sng.data = std::move(data);
        //sng.data = std::move(buff);
        //std::cout << "data buff " << sng.data << std::endl;
        std::cout << "data size " << sng.data.size() << std::endl;
        song_manager s_mgr(exe_path);
        s_mgr.saveSong(sng);
        /* */

        //std::string fileStr = file->getInMemoryData()->c_str();
        //std::cout << fileStr << std::endl;
        //std::cout << "known size " << file->getKnownSize() << std::endl;
        //std::cout << "size " << fileStr.size() << std::endl;

        return createResponse(Status::CODE_200, "OK");
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
    std::string exe_path;

    const long dataSize = 1000000000000;
};

#endif
