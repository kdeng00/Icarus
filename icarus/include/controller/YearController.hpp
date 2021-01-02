#ifndef YEARCONTROLLER_H_
#define YEARCONTROLLER_H_

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
#include "database/YearRepository.h"
#include "dto/conversion/DtoConversions.h"
#include "dto/YearDto.hpp"
// #include "manager/TokenManager.hpp"
#include "manager/Manager.h"
#include "manager/YearManager.h"
#include "type/Scopes.h"
#include "type/YearFilter.h"


namespace controller
{
    class YearController : public BaseController
    {
    public:
		YearController(const icarus_lib::binary_path &bConf, 
                       OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper)) : 
							BaseController(bConf, object_mapper)
        {
        }

		#include OATPP_CODEGEN_BEGIN(ApiController)

		// endpoint for retrieving all year records in json format
		ENDPOINT("GET", "/api/v1/year", yearRecords, 
				 REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

		    auto auth = authHeader->std_str();
		    // manager::TokenManager tok;
		    manager::token_manager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                              type::Scope::retrieveYear),
                              Status::CODE_403, "Not allowed");

		    std::cout << "starting process of retrieving year\n";

		    database::YearRepository yrRepo(m_bConf);
		    auto yrsDb = yrRepo.retrieveRecords();
		    auto yearRecs = oatpp::Vector<oatpp::Object<dto::YearDto>>::createShared();
            yearRecs->reserve(yrsDb.size());

		    for (auto &yrDb : yrsDb) {
		        auto yr = dto::conversion::DtoConversions::toYearDto(yrDb);

				yearRecs->push_back(yr);
		    }

		    return createDtoResponse(Status::CODE_200, yearRecs);
		}

		// endpoint for retrieving single year record by the year id in json format
		ENDPOINT("GET", "/api/v1/year/{id}", yearRecord, 
				 REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id))
        {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

		    auto auth = authHeader->std_str();
		    // manager::TokenManager tok;
		    manager::token_manager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                              type::Scope::retrieveYear),
                              Status::CODE_403, "Not allowed");

		    database::YearRepository yrRepo(m_bConf);
		    icarus_lib::year yrDb(id);

		    OATPP_ASSERT_HTTP(yrRepo.doesYearExist(yrDb, 
                              type::YearFilter::id), 
                              Status::CODE_403, "year does not exist");

		    std::cout << "year exist\n";
		    yrDb = yrRepo.retrieveRecord(yrDb, type::YearFilter::id);

		    auto year = dto::conversion::DtoConversions::toYearDto(yrDb);

		    return createDtoResponse(Status::CODE_200, year);
		}

		#include OATPP_CODEGEN_END(ApiController)
    private:
    };
}

#endif
