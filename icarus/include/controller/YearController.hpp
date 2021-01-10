#ifndef YEARCONTROLLER_H_
#define YEARCONTROLLER_H_

#include <iostream>
#include <limits>

#include <icarus_lib/icarus.h>

#include "controller/BaseController.hpp"
#include "database/YearRepository.h"
#include "dto/conversion/DtoConversions.h"
#include "dto/YearDto.hpp"
#include "manager/Manager.h"
#include "type/Scopes.h"
#include "type/YearFilter.h"

using namespace manager;

using icarus_lib::binary_path;
using oatpp::web::server::handler::DefaultBearerAuthorizationObject;

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
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
            icarus_lib::token tokn;
            tokn.access_token = auth_object->token->std_str();
            token_manager tok(m_bConf);

            auto scope = type::Scope::retrieveYear;
            auto res = tok.is_token_valid(tokn, scope);
            
            OATPP_ASSERT_HTTP(res.first, Status::CODE_403, res.second.c_str());

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
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id))
        {
            icarus_lib::token tokn;
            tokn.access_token = auth_object->token->std_str();
            token_manager tok(m_bConf);

            auto scope = type::Scope::retrieveYear;
            auto res = tok.is_token_valid(tokn, scope);
            
            OATPP_ASSERT_HTTP(res.first, Status::CODE_403, res.second.c_str());

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
