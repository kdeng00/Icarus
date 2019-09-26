#ifndef COVERARTCONTROLLER_H_
#define COVERARTCONTROLLER_H_

#include <filesystem>
#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <memory>
#include <vector>

#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/mime/multipart/InMemoryPartReader.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "database/CoverArtRepository.h"
#include "dto/CoverArtDto.hpp"
#include "manager/CoverArtManager.h"
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/CoverFilter.h"

namespace fs = std::filesystem;

namespace controller {
class CoverArtController : public oatpp::web::server::api::ApiController
{
public:
    CoverArtController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), m_exe_path(p)
    { }

    CoverArtController(const model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf)
    { }

    #include OATPP_CODEGEN_BEGIN(ApiController)

    // endpoint for retrieving all cover art records in json format
    ENDPOINT("GET", "/api/v1/coverart", coverArtRecords, 
        REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        auto authHeader = request->getHeader("Authorization");
        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
        auto auth = authHeader->std_str();
        manager::TokenManager tok;
        OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::downloadCoverArt), Status::CODE_403, "Not allowed");

        std::cout << "starting process of retrieving cover art" << std::endl;
        database::CoverArtRepository covRepo(m_bConf);
        auto covsDb = covRepo.retrieveRecords();
        auto coverArts = oatpp::data::mapping::type::List<dto::CoverArtDto::ObjectWrapper>::createShared();

        for (auto& covDb : covsDb) {
            auto cov = dto::CoverArtDto::createShared();
            cov->id = covDb.id;
            cov->songTitle = covDb.songTitle.c_str();

            coverArts->pushBack(cov);
        }

        return createDtoResponse(Status::CODE_200, coverArts);
    }

    // endpoint for retrieving single cover art record by the cover art id in json format
    ENDPOINT("GET", "/api/v1/coverart/{id}", coverArtRecord, 
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
        auto authHeader = request->getHeader("Authorization");
        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
        auto auth = authHeader->std_str();
        manager::TokenManager tok;
        OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::downloadCoverArt), Status::CODE_403, "Not allowed");

        database::CoverArtRepository covRepo(m_bConf);
        model::Cover covDb;
        covDb.id = id;

        OATPP_ASSERT_HTTP(covRepo.doesCoverArtExist(covDb, type::CoverFilter::id) , Status::CODE_403, "song does not exist");

        std::cout << "cover art exists" << std::endl;
        covDb = covRepo.retrieveRecord(covDb, type::CoverFilter::id);

        auto coverArt = dto::CoverArtDto::createShared();
        coverArt->id = covDb.id;
        coverArt->songTitle = covDb.songTitle.c_str();

        return createDtoResponse(Status::CODE_200, coverArt);
    }

    ENDPOINT("GET", "/api/v1/coverart/download/{id}", downloadCoverArt, 
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
        auto authHeader = request->getHeader("Authorization");
        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
        auto auth = authHeader->std_str();
        manager::TokenManager tok;
        OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::downloadCoverArt), Status::CODE_403, "Not allowed");

        database::CoverArtRepository covRepo(m_bConf);
        model::Cover covDb;
        covDb.id = id;
        covDb = covRepo.retrieveRecord(covDb, type::CoverFilter::id);

        auto rawCover = oatpp::base::StrBuffer::loadFromFile(covDb.imagePath.c_str());
        auto response = createResponse(Status::CODE_200, rawCover);
            
        response->putHeader(Header::CONTENT_TYPE, "image/*");

        return response;
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
    std::string m_exe_path;
    model::BinaryPath m_bConf;
};
}
#endif
