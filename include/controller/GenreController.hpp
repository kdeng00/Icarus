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
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/GenreFilter.h"

namespace fs = std::filesystem;

namespace controller
{
    class GenreController : public oatpp::web::server::api::ApiController
    {
    public:
        GenreController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper), m_exe_path(p)
        { }

        GenreController(const model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf)
        { }

        #include OATPP_CODEGEN_BEGIN(ApiController)

        // endpoint for retrieving all genre records in json format
        ENDPOINT("GET", "/api/v1/genre", genreRecords, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
            std::cout << "starting process of retrieving genre" << std::endl;
            database::GenreRepository gnrRepo(m_bConf);
            auto gnrsDb = gnrRepo.retrieveRecords();
            auto genres = oatpp::data::mapping::type::List<dto::GenreDto::ObjectWrapper>::createShared();

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
            PATH(Int32, id)) {

            database::GenreRepository gnrRepo(m_bConf);
            model::Genre gnrDb(id);

            OATPP_ASSERT_HTTP(gnrRepo.doesGenreExist(gnrDb, type::GenreFilter::id) , Status::CODE_403, "genre does not exist");

            std::cout << "genre exist" << std::endl;
            gnrDb = gnrRepo.retrieveRecord(gnrDb, type::GenreFilter::id);

            auto genre = dto::GenreDto::createShared();
            genre->id = gnrDb.id;
            genre->category= gnrDb.category.c_str();

            return createDtoResponse(Status::CODE_200, genre);
        }

        #include OATPP_CODEGEN_END(ApiController)
    private:
        std::string m_exe_path;
        model::BinaryPath m_bConf;
        const long m_dataSize = std::numeric_limits<long long int>::max();
    };
}
#endif
