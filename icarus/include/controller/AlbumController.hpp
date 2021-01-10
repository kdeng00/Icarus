#ifndef ALBUMCONTROLLER_H_
#define ALBUMCONTROLLER_H_

#include <iostream>
#include <limits>

#include <icarus_lib/icarus.h>

#include "controller/BaseController.hpp"
#include "database/Repositories.h"
#include "dto/AlbumDto.hpp"
#include "dto/conversion/DtoConversions.h"
#include "manager/Manager.h"
#include "type/Scopes.h"
#include "type/AlbumFilter.h"

using namespace dto;
using manager::token_manager;

namespace controller
{
    class AlbumController : public BaseController
    {
    public:
        AlbumController(const icarus_lib::binary_path& bConf, 
                        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper)) : 
                            BaseController(bConf, object_mapper)
        {
        }

        #include OATPP_CODEGEN_BEGIN(ApiController)

        // endpoint for retrieving all album records in json format
        ENDPOINT("GET", "/api/v1/album", albumRecords, 
                 REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
            auto authHeader = request->getHeader("Authorization");

            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

            auto auth = authHeader->std_str();

            token_manager tok;

            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                        type::Scope::retrieveAlbum), Status::CODE_403, "Not allowed");

            std::cout << "starting process of retrieving album\n";

            database::album_repo albRepo(m_bConf);

            auto albsDb = albRepo.retrieveRecords();

            auto albums = oatpp::Vector<oatpp::Object<AlbumDto>>::createShared();
            albums->reserve(albsDb.size());

            for (auto& albDb : albsDb) {
                auto alb = dto::conversion::DtoConversions::toAlbumDto(albDb);

                albums->push_back(alb);
            }

            return createDtoResponse(Status::CODE_200, albums);
        }

        // endpoint for retrieving single album record by the album id in json format
        ENDPOINT("GET", "/api/v1/album/{id}", albumRecord, 
                 REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id))
        {
            auto authHeader = request->getHeader("Authorization");

            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

            auto auth = authHeader->std_str();

            token_manager tok;

            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                        type::Scope::retrieveAlbum), Status::CODE_403, "Not allowed");

            database::album_repo albRepo(m_bConf);
            icarus_lib::album albDb(id);

            OATPP_ASSERT_HTTP(albRepo.doesAlbumExists(albDb, 
                        type::AlbumFilter::id) , Status::CODE_403, "album does not exist");

            std::cout << "album exists\n";
            albDb = albRepo.retrieveRecord(albDb, type::AlbumFilter::id);

            auto album = dto::conversion::DtoConversions::toAlbumDto(albDb);

            return createDtoResponse(Status::CODE_200, album);
        }

        #include OATPP_CODEGEN_END(ApiController)
    private:
    };
}
#endif
