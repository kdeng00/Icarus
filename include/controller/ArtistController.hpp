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
#include "dto/conversion/DtoConversions.h"
#include "manager/ArtistManager.h"
#include "manager/TokenManager.h"
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/ArtistFilter.h"

namespace fs = std::filesystem;

namespace controller
{
    class ArtistController : public oatpp::web::server::api::ApiController
    {
    public:
		ArtistController(const model::BinaryPath& bConf, 
                         OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper)) : 
                            oatpp::web::server::api::ApiController(objectMapper), 
                            m_bConf(bConf)
        {
        }

		#include OATPP_CODEGEN_BEGIN(ApiController)

		// endpoint for retrieving all artist records in json format
		ENDPOINT("GET", "/api/v1/artist", artistRecords, 
				 REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
		    auto authHeader = request->getHeader("Authorization");

		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();

		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                              type::Scope::retrieveArtist), 
                              Status::CODE_403,
                              "Not allowed");

		    std::cout << "starting process of retrieving artist\n";

		    database::ArtistRepository artRepo(m_bConf);
		    auto artsDb = artRepo.retrieveRecords();
		    auto artists = oatpp::Vector<oatpp::Object<dto::ArtistDto>>::createShared();
            artists->reserve(artsDb.size());
                    

		    for (auto& artDb : artsDb) {
                auto art = dto::conversion::DtoConversions::toArtistDto(artDb);

				artists->push_back(art);
		    }

		    return createDtoResponse(Status::CODE_200, artists);
		}

		// endpoint for retrieving single artist record by the artist id in json format
		ENDPOINT("GET", "/api/v1/artist/{id}", artistRecord, 
				 REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id))
        {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

		    auto auth = authHeader->std_str();

		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                              type::Scope::retrieveArtist),
                              Status::CODE_403, 
                              "Not allowed");

		    database::ArtistRepository artRepo(m_bConf);
		    model::Artist artDb(id);

		    OATPP_ASSERT_HTTP(artRepo.doesArtistExist(artDb, 
                        type::ArtistFilter::id) , Status::CODE_403, "artist does not exist");

		    std::cout << "artist exist\n";

		    artDb = artRepo.retrieveRecord(artDb, type::ArtistFilter::id);

		    auto artist = dto::conversion::DtoConversions::toArtistDto(artDb);

		    return createDtoResponse(Status::CODE_200, artist);
		}

		#include OATPP_CODEGEN_END(ApiController)
    private:
		model::BinaryPath m_bConf;
    };
}
#endif
