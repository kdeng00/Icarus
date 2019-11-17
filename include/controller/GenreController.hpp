#ifndef GENRECONTROLLER_H_
#define GENRECONTROLLER_H_

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

#include "database/GenreRepository.h"
#include "dto/GenreDto.hpp"
#include "manager/GenreManager.h"
#include "manager/YearManager.h"
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/GenreFilter.h"

namespace fs = std::filesystem;

namespace controller {
    class GenreController : public oatpp::web::server::api::ApiController {
    public:
		GenreController(const model::BinaryPath& bConf, 
                OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		        : oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf) { }

		#include OATPP_CODEGEN_BEGIN(ApiController)

		// endpoint for retrieving all genre records in json format
		ENDPOINT("GET", "/api/v1/genre", genreRecords, 
				REQUEST(std::shared_ptr<IncomingRequest>, request)) {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::retrieveGenre), Status::CODE_403, "Not allowed");

		    std::cout << "starting process of retrieving genre\n";
		    database::GenreRepository gnrRepo(m_bConf);
		    auto gnrsDb = gnrRepo.retrieveRecords();
		    auto genres = oatpp::data::mapping::type::
                    List<dto::GenreDto::ObjectWrapper>::createShared();

		    for (auto& gnrDb : gnrsDb) {
		        auto gnr = dto::GenreDto::createShared();
		        gnr->id = gnrDb.id;
		        gnr->category = gnrDb.category.c_str();

		        genres->pushBack(gnr);
		    }

		    return createDtoResponse(Status::CODE_200, genres);
		}

		// endpoint for retrieving single genre record by the genre id in json format
		ENDPOINT("GET", "/api/v1/genre/{id}", genreRecord, 
				REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::retrieveGenre), Status::CODE_403, "Not allowed");

		    database::GenreRepository gnrRepo(m_bConf);
		    model::Genre gnrDb(id);

		    OATPP_ASSERT_HTTP(gnrRepo.doesGenreExist(gnrDb, 
                    type::GenreFilter::id) , Status::CODE_403, "genre does not exist");

		    std::cout << "genre exist\n";
		    gnrDb = gnrRepo.retrieveRecord(gnrDb, type::GenreFilter::id);

		    auto genre = dto::GenreDto::createShared();
		    genre->id = gnrDb.id;
		    genre->category= gnrDb.category.c_str();

		    return createDtoResponse(Status::CODE_200, genre);
		}

		#include OATPP_CODEGEN_END(ApiController)
    private:
		model::BinaryPath m_bConf;
    };
}
#endif
