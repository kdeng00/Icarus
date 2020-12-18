#ifndef YEARCONTROLLER_H_
#define YEARCONTROLLER_H_

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

#include "database/YearRepository.h"
#include "dto/YearDto.hpp"
#include "manager/YearManager.h"
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/YearFilter.h"

namespace fs = std::filesystem;

namespace controller {
    class YearController : public oatpp::web::server::api::ApiController {
    public:
		YearController(const model::BinaryPath& bConf, 
                OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
				: oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf) { }

		#include OATPP_CODEGEN_BEGIN(ApiController)

		// endpoint for retrieving all year records in json format
		ENDPOINT("GET", "/api/v1/year", yearRecords, 
				REQUEST(std::shared_ptr<IncomingRequest>, request)) {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::retrieveYear), Status::CODE_403, "Not allowed");

		    std::cout << "starting process of retrieving year\n";
		    database::YearRepository yrRepo(m_bConf);
		    auto yrsDb = yrRepo.retrieveRecords();
		    // auto yearRecs = oatpp::data::mapping::type::
                    // List<dto::YearDto::ObjectWrapper>::createShared();
                    // List<dto::YearDto>::createShared();
		    auto yearRecs = oatpp::Vector<oatpp::Object<dto::YearDto>>::createShared();
                    // List<dto::YearDto::ObjectWrapper>::createShared();
                    // List<dto::YearDto>::createShared();

		    for (auto& yrDb : yrsDb) {
		        auto yr = dto::YearDto::createShared();
		        yr->id = yrDb.id;
		        yr->year = yrDb.year;

		        // yearRecs->pushBack(yr);
				yearRecs->push_back(yr);
		    }

		    return createDtoResponse(Status::CODE_200, yearRecs);
		}

		// endpoint for retrieving single year record by the year id in json format
		ENDPOINT("GET", "/api/v1/year/{id}", yearRecord, 
				REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::retrieveYear), Status::CODE_403, "Not allowed");

		    database::YearRepository yrRepo(m_bConf);
		    model::Year yrDb(id);

		    OATPP_ASSERT_HTTP(yrRepo.doesYearExist(yrDb, 
                    type::YearFilter::id) , Status::CODE_403, "year does not exist");

		    std::cout << "year exist\n";
		    yrDb = yrRepo.retrieveRecord(yrDb, type::YearFilter::id);

		    auto year = dto::YearDto::createShared();
		    year->id = yrDb.id;
		    year->year= yrDb.year;

		    return createDtoResponse(Status::CODE_200, year);
		}

		#include OATPP_CODEGEN_END(ApiController)
    private:
		model::BinaryPath m_bConf;
    };
}

#endif
