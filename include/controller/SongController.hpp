#ifndef SONGCONTROLLER_H_
#define SONGCONTROLLER_H_

#include <filesystem>
#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <memory>
#include <vector>

#include <oatpp/core/async/Coroutine.hpp>
#include "oatpp/core/data/stream/ChunkedBuffer.hpp"
#include "oatpp/core/data/stream/FileStream.hpp"
#include "oatpp/core/base/StrBuffer.hpp"
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/macro/component.hpp"
#include "oatpp/web/mime/multipart/InMemoryPartReader.hpp"
#include "oatpp/web/mime/multipart/Reader.hpp"
#include "oatpp/web/protocol/http/outgoing/StreamingBody.hpp"
#include "oatpp/web/server/api/ApiController.hpp"

#include "callback/StreamCallback.h"
#include "database/SongRepository.h"
#include "dto/SongDto.hpp"
#include "dto/conversion/DtoConversions.h"
#include "manager/SongManager.h"
#include "manager/TokenManager.h"
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/SongFilter.h"
#include "type/SongUpload.h"

namespace fs = std::filesystem;

namespace controller {
    class SongController : public oatpp::web::server::api::ApiController {
    public:
		SongController(const model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
		    : oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf) { }

		#include OATPP_CODEGEN_BEGIN(ApiController)

		// endpoint for uploading a song
		ENDPOINT("POST", "/api/v1/song/data", songUpload, 
                //AUTHORIZATION(std::shared_ptr<oatpp::web::server::handler::DefaultBearerAuthorizationObject>, authObject),
				REQUEST(std::shared_ptr<IncomingRequest>, request)) {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
            
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::upload), Status::CODE_403, "Not allowed");

		    auto mp = 
                std::make_shared<oatpp::web::mime::multipart::Multipart>
                (request->getHeaders());

		    oatpp::web::mime::multipart::Reader mp_reader(mp.get());

		    mp_reader.setPartReader("file", 
                    oatpp::web::mime::multipart::createInMemoryPartReader(m_dataSize));

		    request->transferBody(&mp_reader);

		    auto file = mp->getNamedPart("file");

		    OATPP_ASSERT_HTTP(file, Status::CODE_400, "file is null");

            auto buff = std::make_unique<const char*>(file->getInMemoryData()->c_str());
            auto buffSize = file->getKnownSize();

		    std::vector<unsigned char> data(*buff, *buff + buffSize);

		    model::Song sng;
		    sng.data = std::move(data);
		
		    manager::SongManager songMgr(m_bConf);
		    const auto result =  songMgr.saveSong(sng);
		    if (!result.first) {
		        switch (result.second) {
		            case type::SongUpload::AlreadyExist:
		                return createResponse(Status::CODE_400, "Song already exists");
		            default:
		                break;
		        }
		    }

		    auto songDto = dto::conversion::DtoConversions::toSongDto(sng);

		    return createDtoResponse(Status::CODE_200, songDto);
		}

		// endpoint for retrieving all song records in json format
		ENDPOINT("GET", "/api/v1/song", songRecords, 
				REQUEST(std::shared_ptr<IncomingRequest>, request)) {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::retrieveSong), Status::CODE_403, "Not allowed");

		    std::cout << "starting process of retrieving songs\n";
		    database::SongRepository songRepo(m_bConf);
		    auto songsDb = songRepo.retrieveRecords();
		    // auto songs = oatpp::data::mapping::type::List<dto::SongDto::ObjectWrapper>::createShared();
		    auto songs = oatpp::Vector<oatpp::Object<dto::SongDto>>::createShared();

		    // auto yearRecs = oatpp::Vector<oatpp::Object<dto::YearDto>>::createShared();

		    std::cout << "creating object to send\n";
		    for (auto& songDb : songsDb) {
		        auto song = dto::conversion::DtoConversions::toSongDto(songDb);

		        songs->push_back(song);
		    }

		    return createDtoResponse(Status::CODE_200, songs);
		}

        // endpoint for retrieving song record by the song id in json format
		ENDPOINT("GET", "/api/v1/song/{id}", songRecord,
				REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::retrieveSong), Status::CODE_403, "Not allowed");

		    database::SongRepository songRepo(m_bConf);
		    model::Song songDb(id);

		    if (!songRepo.doesSongExist(songDb, type::SongFilter::id)) {
		        return songDoesNotExist();
		    }

		    std::cout << "song exists\n";
		    songDb = songRepo.retrieveRecord(songDb, type::SongFilter::id);

		    auto song = dto::conversion::DtoConversions::toSongDto(songDb);

		    return createDtoResponse(Status::CODE_200, song);
		}

		ENDPOINT("GET", "/api/v1/song/data/{id}", downloadSong, 
				REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
    		auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::download), Status::CODE_403, "Not allowed");

		    database::SongRepository songRepo(m_bConf);
		    model::Song songDb(id);
		    if (!songRepo.doesSongExist(songDb, type::SongFilter::id)) {
		        return songDoesNotExist();
		    }

		    songDb = songRepo.retrieveRecord(songDb, type::SongFilter::id);

		    auto rawSong = oatpp::base::StrBuffer::loadFromFile(songDb.songPath.c_str());
		    
		    auto response = createResponse(Status::CODE_200, rawSong);
		    response->putHeader(Header::CONTENT_TYPE, "audio/mpeg");

		    return response;
		}

		ENDPOINT("UPDATE", "/api/v1/song/{id}", songUpdate,
				REQUEST(std::shared_ptr<IncomingRequest>, request),
    		    // BODY_DTO(dto::SongDto::ObjectWrapper, songDto), PATH(Int32, id)) {
    		    BODY_DTO(oatpp::data::mapping::type::ObjectWrapper<dto::SongDto>, songDto), PATH(Int32, id)) {
		    songDto->id = id;
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::updateSong), Status::CODE_403, "Not allowed");
            
		    auto updatedSong = dto::conversion::DtoConversions::toSong(songDto);
		    manager::SongManager songMgr(m_bConf);
		    auto result = songMgr.updateSong(updatedSong);
		    if (!result) {
		        return songDoesNotExist();
		    }

		    return createResponse(Status::CODE_200, "OK");
		}

		// endpoint to delete a song
		ENDPOINT("DELETE", "api/v1/song/data/{id}", songDelete, 
				REQUEST(std::shared_ptr<IncomingRequest>, request),   PATH(Int32, id)) {
    		auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::deleteSong), Status::CODE_403, "Not allowed");

		    model::Song song(id);

		    manager::SongManager sngMgr(m_bConf);
		    auto result = sngMgr.deleteSong(song);
		    if (!result) {
		        return songDoesNotExist();
		    }

		    return createResponse(Status::CODE_200, "OK");
		}

		// endpoint for streaming a song
		ENDPOINT("GET", "/api/v1/song/stream/{id}", streamSong,
				REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id)) {
		    auto authHeader = request->getHeader("Authorization");
		    OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");
		    auto auth = authHeader->std_str();
		    manager::TokenManager tok;
		    OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::stream), Status::CODE_403, "Not allowed");

		    database::SongRepository songRepo(m_bConf);
		    model::Song songDb(id);
		    if (!songRepo.doesSongExist(songDb, type::SongFilter::id)) {
		        return songDoesNotExist();
		    }

		    songDb = songRepo.retrieveRecord(songDb, type::SongFilter::id);

		    auto dSize = 1024;

            auto callback = std::make_shared<callback::StreamCallback>(songDb.songPath);
		    auto db = std::make_shared<oatpp::web::protocol::http::outgoing::StreamingBody>(
                    callback
                    );

		    auto response = OutgoingResponse::createShared(Status::CODE_200, db);
		    response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
		    response->putHeader(Header::CONTENT_TYPE, "audio/mpeg");

		    return response;
		}

		#include OATPP_CODEGEN_END(ApiController)
    private:
		std::shared_ptr<oatpp::web::protocol::http::outgoing::Response> 
		    songDoesNotExist() {

		    return createResponse(Status::CODE_404, "Song not found");
		}

		model::BinaryPath m_bConf;

		const long m_dataSize = std::numeric_limits<long long int>::max();
    };
}
#endif
