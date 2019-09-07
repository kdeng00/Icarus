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

namespace controller
{
    class YearController : public oatpp::web::server::api::ApiController
    {
    public:
        YearController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper), m_exe_path(p)
        { }

        YearController(const model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf)
        { }

        #include OATPP_CODEGEN_BEGIN(ApiController)

        // endpoint for retrieving all year records in json format
        ENDPOINT("GET", "/api/v1/year", yearRecords, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
            std::cout << "starting process of retrieving year" << std::endl;
            database::YearRepository yrRepo(m_bConf);
            auto yrsDb = yrRepo.retrieveRecords();
            auto yearRecs = oatpp::data::mapping::type::List<dto::YearDto::ObjectWrapper>::createShared();

            for (auto& yrDb : yrsDb) {
                auto yr = dto::YearDto::createShared();
                yr->id = yrDb.id;
                yr->year = yrDb.year;

                yearRecs->pushBack(yr);
            }

            return createDtoResponse(Status::CODE_200, yearRecs);
        }

        // endpoint for retrieving single year record by the year id in json format
        ENDPOINT("GET", "/api/v1/year/{id}", yearRecord, 
            PATH(Int32, id)) {

            database::YearRepository yrRepo(m_bConf);
            model::Year yrDb(id);

            OATPP_ASSERT_HTTP(yrRepo.doesYearExist(yrDb, type::YearFilter::id) , Status::CODE_403, "year does not exist");

            std::cout << "year exist" << std::endl;
            yrDb = yrRepo.retrieveRecord(yrDb, type::YearFilter::id);

            auto year = dto::YearDto::createShared();
            year->id = yrDb.id;
            year->year= yrDb.year;

            return createDtoResponse(Status::CODE_200, year);
        }

        #include OATPP_CODEGEN_END(ApiController)
    private:
        std::string m_exe_path;
        model::BinaryPath m_bConf;
        const long m_dataSize = std::numeric_limits<long long int>::max();
    };
}
#endif
