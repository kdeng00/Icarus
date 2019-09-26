#ifndef ALBUMCONTROLLER_H_
#define ALBUMCONTROLLER_H_

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

#include "database/AlbumRepository.h"
#include "dto/AlbumDto.hpp"
#include "manager/AlbumManager.h"
#include "manager/TokenManager.h"
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/AlbumFilter.h"

namespace fs = std::filesystem;

namespace controller {
class AlbumController : public oatpp::web::server::api::ApiController
{
public:
    AlbumController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), m_exe_path(p)
    { }

    AlbumController(const model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf)
    { }

    #include OATPP_CODEGEN_BEGIN(ApiController)

    // endpoint for retrieving all album records in json format
    ENDPOINT("GET", "/api/v1/album", albumRecords, 
        REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        auto authHeader = request->getHeader("Authorization");
        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
        auto auth = authHeader->std_str();
        manager::TokenManager tok;
        OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::retrieveAlbum), Status::CODE_403, "Not allowed");

        std::cout << "starting process of retrieving album" << std::endl;
        database::AlbumRepository albRepo(m_bConf);
        auto albsDb = albRepo.retrieveRecords();
        auto albums = oatpp::data::mapping::type::List<dto::AlbumDto::ObjectWrapper>::createShared();

        for (auto& albDb : albsDb) {
            auto alb = dto::AlbumDto::createShared();
            alb->id = albDb.id;
            alb->title = albDb.title.c_str();
            alb->year = albDb.year;

            albums->pushBack(alb);
        }

        return createDtoResponse(Status::CODE_200, albums);
    }

    // endpoint for retrieving single album record by the album id in json format
    ENDPOINT("GET", "/api/v1/album/{id}", albumRecord, 
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
        auto authHeader = request->getHeader("Authorization");
        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
        auto auth = authHeader->std_str();
        manager::TokenManager tok;
        OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::retrieveAlbum), Status::CODE_403, "Not allowed");

        database::AlbumRepository albRepo(m_bConf);
        model::Album albDb(id);

        OATPP_ASSERT_HTTP(albRepo.doesAlbumExists(albDb, type::AlbumFilter::id) , Status::CODE_403, "album does not exist");

        std::cout << "album exists" << std::endl;
        albDb = albRepo.retrieveRecord(albDb, type::AlbumFilter::id);

        auto album = dto::AlbumDto::createShared();
        album->id = albDb.id;
        album->title = albDb.title.c_str();
        album->year = albDb.year;

        return createDtoResponse(Status::CODE_200, album);
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
    std::string m_exe_path;
    model::BinaryPath m_bConf;
};
}
#endif
