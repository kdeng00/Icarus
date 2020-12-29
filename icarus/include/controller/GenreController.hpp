#ifndef GENRECONTROLLER_H_
#define GENRECONTROLLER_H_

#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <memory>
#include <vector>

#include "icarus_lib/icarus.h"
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/mime/multipart/InMemoryPartReader.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "controller/BaseController.hpp"
#include "database/GenreRepository.h"
#include "dto/GenreDto.hpp"
#include "dto/conversion/DtoConversions.h"
#include "manager/GenreManager.h"
#include "manager/TokenManager.h"
#include "manager/YearManager.h"
#include "type/Scopes.h"
#include "type/GenreFilter.h"

namespace controller
{
    class GenreController : public BaseController
    {
    public:
		GenreController(const icarus_lib::binary_path &bConf, 
                        OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper)) : 
							BaseController(bConf, object_mapper)
        {
        }

		#include OATPP_CODEGEN_BEGIN(ApiController)

		// endpoint for retrieving all genre records in json format
		ENDPOINT("GET", "/api/v1/genre", genreRecords, 
				REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::retrieveGenre), Status::CODE_403, "Not allowed");

		    std::cout << "starting process of retrieving genre\n";

		    database::GenreRepository gnrRepo(m_bConf);
		    auto gnrsDb = gnrRepo.retrieveRecords();
		    auto genres = oatpp::Vector<oatpp::Object<dto::GenreDto>>::createShared();
            genres->reserve(gnrsDb.size());

		    for (auto& gnrDb : gnrsDb) {
		        auto gnr = dto::conversion::DtoConversions::toGenreDto(gnrDb);

		        genres->push_back(gnr);
		    }

		    return createDtoResponse(Status::CODE_200, genres);
		}

		// endpoint for retrieving single genre record by the genre id in json format
		ENDPOINT("GET", "/api/v1/genre/{id}", genreRecord, 
				 REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id))
        {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::retrieveGenre), Status::CODE_403, "Not allowed");

		    database::GenreRepository gnrRepo(m_bConf);
		    icarus_lib::genre gnrDb(id);

		    OATPP_ASSERT_HTTP(gnrRepo.doesGenreExist(gnrDb, 
                    type::GenreFilter::id) , Status::CODE_403, "genre does not exist");

		    std::cout << "genre exist\n";
		    gnrDb = gnrRepo.retrieveRecord(gnrDb, type::GenreFilter::id);

		    auto genre = dto::conversion::DtoConversions::toGenreDto(gnrDb);

		    return createDtoResponse(Status::CODE_200, genre);
		}

		#include OATPP_CODEGEN_END(ApiController)
    private:
    };
}
#endif
