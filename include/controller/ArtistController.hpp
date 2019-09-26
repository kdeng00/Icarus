#ifndef ARTISTCONTROLLER_H_
#define ARTISTCONTROLLER_H_

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

#include "database/ArtistRepository.h"
#include "dto/ArtistDto.hpp"
#include "manager/ArtistManager.h"
#include "manager/TokenManager.h"
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/ArtistFilter.h"

namespace fs = std::filesystem;

namespace controller {
class ArtistController : public oatpp::web::server::api::ApiController
{
public:
    ArtistController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), m_exe_path(p)
    { }

    ArtistController(const model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf)
    { }

    #include OATPP_CODEGEN_BEGIN(ApiController)

    // endpoint for retrieving all artist records in json format
    ENDPOINT("GET", "/api/v1/artist", artistRecords, 
        REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        auto authHeader = request->getHeader("Authorization");
        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
        auto auth = authHeader->std_str();
        manager::TokenManager tok;
        OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::retrieveArtist), Status::CODE_403, "Not allowed");

        std::cout << "starting process of retrieving artist" << std::endl;
        database::ArtistRepository artRepo(m_bConf);
        auto artsDb = artRepo.retrieveRecords();
        auto artists = oatpp::data::mapping::type::List<dto::ArtistDto::ObjectWrapper>::createShared();

        for (auto& artDb : artsDb) {
            auto art = dto::ArtistDto::createShared();
            art->id = artDb.id;
            art->artist = artDb.artist.c_str();

            artists->pushBack(art);
        }

        return createDtoResponse(Status::CODE_200, artists);
    }

    // endpoint for retrieving single artist record by the artist id in json format
    ENDPOINT("GET", "/api/v1/artist/{id}", artistRecord, 
        REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
        auto authHeader = request->getHeader("Authorization");
        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
        auto auth = authHeader->std_str();
        manager::TokenManager tok;
        OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::retrieveArtist), Status::CODE_403, "Not allowed");

        database::ArtistRepository artRepo(m_bConf);
        model::Artist artDb(id);

        OATPP_ASSERT_HTTP(artRepo.doesArtistExist(artDb, type::ArtistFilter::id) , Status::CODE_403, "artist does not exist");

        std::cout << "artist exist" << std::endl;
        artDb = artRepo.retrieveRecord(artDb, type::ArtistFilter::id);

        auto artist = dto::ArtistDto::createShared();
        artist->id = artDb.id;
        artist->artist = artDb.artist.c_str();

        return createDtoResponse(Status::CODE_200, artist);
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
    std::string m_exe_path;
    model::BinaryPath m_bConf;
};
}
#endif
