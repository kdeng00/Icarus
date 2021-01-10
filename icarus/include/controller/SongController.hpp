#ifndef SONGCONTROLLER_H_
#define SONGCONTROLLER_H_

#include <iostream>
#include <limits>
#include <vector>

#include <icarus_lib/icarus.h>
#include <oatpp/core/data/stream/ChunkedBuffer.hpp>
#include <oatpp/core/data/stream/FileStream.hpp>
#include <oatpp/core/base/StrBuffer.hpp>
#include <oatpp/web/mime/multipart/InMemoryPartReader.hpp>
#include <oatpp/web/mime/multipart/Multipart.hpp>
#include <oatpp/web/mime/multipart/PartList.hpp>
#include <oatpp/web/mime/multipart/Reader.hpp>
#include <oatpp/web/protocol/http/outgoing/StreamingBody.hpp>

#include "callback/StreamCallback.h"
#include "controller/BaseController.hpp"
#include "database/Repositories.h"
#include "dto/SongDto.hpp"
#include "dto/conversion/DtoConversions.h"
#include "manager/Manager.h"
#include "type/Scopes.h"
#include "type/SongFilter.h"
#include "type/SongUpload.h"


using namespace dto;
using namespace manager;

using oatpp::web::server::handler::DefaultBearerAuthorizationObject;
using oatpp::web::protocol::http::outgoing::Response;
using oatpp::web::mime::multipart::PartList;

using database::song_repo;


namespace controller
{
    class SongController : public BaseController
    {
    public:
        SongController(const icarus_lib::binary_path &bConf, 
                       OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, object_mapper)) : 
                            BaseController(bConf, object_mapper)
        {
        }

        #include OATPP_CODEGEN_BEGIN(ApiController)

        // endpoint for uploading a song
        ENDPOINT("POST", "/api/v1/song/data", songUpload, 
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
            /**
            auto authHeader = request->getHeader("Authorization");
            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Empty Authorization key");

            auto auth = authHeader->std_str();
            token_manager tok;
            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                              type::Scope::upload), 
                              Status::CODE_403, "Not allowed");
            */

            icarus_lib::token tokn;
            tokn.access_token = auth_object->token->std_str();
            token_manager tok(m_bConf);

            auto scope = type::Scope::upload;
            auto res = tok.is_token_valid(tokn, scope);
            
            OATPP_ASSERT_HTTP(res.first, Status::CODE_403, res.second.c_str());

            auto mp = std::make_shared<PartList>(request->getHeaders());

            oatpp::web::mime::multipart::Reader mp_reader = mp.get();

            constexpr auto reader_size = 30 * 1024 * 1024;

            auto create_mem = oatpp::web::mime::multipart::createInMemoryPartReader;
            mp_reader.setDefaultPartReader(create_mem(reader_size));

            request->transferBody(&mp_reader);

            auto file = mp->getNamedPart("file");

            OATPP_ASSERT_HTTP(file, Status::CODE_400, "file is null");

            auto buff = std::make_unique<const char*>(file->getInMemoryData()->c_str());
            auto buffSize = file->getKnownSize();

            std::vector<unsigned char> data(*buff, *buff + buffSize);

            icarus_lib::song sng;
            sng.data = std::move(data);
        
            song_manager songMgr(m_bConf);
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
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
            auto authHeader = request->getHeader("Authorization");
            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, no_auth_header_key());

            auto auth = authHeader->std_str();
            token_manager tok;
            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::retrieveSong), 
                              Status::CODE_403, "Not allowed");

            std::cout << "starting process of retrieving songs\n";
            song_repo songRepo(m_bConf);
            auto songsDb = songRepo.retrieveRecords();
            auto songs = oatpp::Vector<oatpp::Object<dto::SongDto>>::createShared();


            std::cout << "creating object to send\n";

            for (auto& songDb : songsDb) {
                auto song = dto::conversion::DtoConversions::toSongDto(songDb);

                songs->push_back(song);
            }

            return createDtoResponse(Status::CODE_200, songs);
        }

        // endpoint for retrieving song record by the song id in json format
        ENDPOINT("GET", "/api/v1/song/{id}", songRecord,
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id))
        {
            icarus_lib::token tokn;
            tokn.access_token = auth_object->token->std_str();
            token_manager tok(m_bConf);

            auto scope = type::Scope::retrieveSong;
            auto res = tok.is_token_valid(tokn, scope);
            
            OATPP_ASSERT_HTTP(res.first, Status::CODE_403, res.second.c_str());

            song_repo songRepo(m_bConf);
            icarus_lib::song songDb(id);

            if (!songRepo.doesSongExist(songDb, type::SongFilter::id)) {
                return songDoesNotExist();
            }

            std::cout << "song exists\n";
            songDb = songRepo.retrieveRecord(songDb, type::SongFilter::id);

            auto song = dto::conversion::DtoConversions::toSongDto(songDb);

            return createDtoResponse(Status::CODE_200, song);
        }

        ENDPOINT("GET", "/api/v1/song/data/{id}", downloadSong, 
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id))
        {
            auto authHeader = request->getHeader("Authorization");
            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

            auto auth = authHeader->std_str();
            token_manager tok;
            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::download), 
                              Status::CODE_403, "Not allowed");


            song_repo songRepo(m_bConf);
            icarus_lib::song songDb(id);

            if (!songRepo.doesSongExist(songDb, type::SongFilter::id)) {
                return songDoesNotExist();
            }

            songDb = songRepo.retrieveRecord(songDb, type::SongFilter::id);

            auto rawSong = oatpp::base::StrBuffer::loadFromFile(songDb.song_path.c_str());
            
            auto response = createResponse(Status::CODE_200, rawSong);
            response->putHeader(Header::CONTENT_TYPE, "audio/mpeg");

            return response;
        }

        ENDPOINT("UPDATE", "/api/v1/song/{id}", songUpdate,
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request),
                 BODY_DTO(oatpp::Object<SongDto>, songDto), PATH(Int32, id))
        {
            songDto->id = id;
            auto authHeader = request->getHeader("Authorization");
            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

            auto auth = authHeader->std_str();
            token_manager tok;
            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, 
                    type::Scope::updateSong), Status::CODE_403, "Not allowed");
            
            auto updatedSong = dto::conversion::DtoConversions::toSong(songDto);
            song_manager songMgr(m_bConf);
            auto result = songMgr.updateSong(updatedSong);

            if (!result) {
                return songDoesNotExist();
            }

            return createResponse(Status::CODE_200, "OK");
        }

        // endpoint to delete a song
        ENDPOINT("DELETE", "api/v1/song/data/{id}", songDelete, 
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request),   PATH(Int32, id))
        {
            auto authHeader = request->getHeader("Authorization");
            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

            auto auth = authHeader->std_str();
            token_manager tok;
            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::deleteSong), 
                              Status::CODE_403, "Not allowed");

            icarus_lib::song song(id);

            song_manager sngMgr(m_bConf);
            auto result = sngMgr.deleteSong(song);

            if (!result) {
                return songDoesNotExist();
            }

            return createResponse(Status::CODE_200, "OK");
        }

        // endpoint for streaming a song
        ENDPOINT("GET", "/api/v1/song/stream/{id}", streamSong,
                 AUTHORIZATION(std::shared_ptr<DefaultBearerAuthorizationObject>, auth_object),
                 REQUEST(std::shared_ptr<IncomingRequest>, request), PATH(Int32, id))
        {
            /**
            auto authHeader = request->getHeader("Authorization");
            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

            auto auth = authHeader->std_str();
            token_manager tok;
            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::stream), 
                              Status::CODE_403, "Not allowed");
            */

            icarus_lib::token tokn;
            tokn.access_token = auth_object->token->std_str();
            token_manager tok(m_bConf);

            auto scope = type::Scope::stream;
            auto res = tok.is_token_valid(tokn, scope);
            
            OATPP_ASSERT_HTTP(res.first, Status::CODE_403, res.second.c_str());

            song_repo songRepo(m_bConf);
            icarus_lib::song songDb(id);

            if (!songRepo.doesSongExist(songDb, type::SongFilter::id)) {
                return songDoesNotExist();
            }

            songDb = songRepo.retrieveRecord(songDb, type::SongFilter::id);

            constexpr auto dSize = 1024;

            auto callback = std::make_shared<callback::StreamCallback>(songDb.song_path);
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
        std::shared_ptr<Response> songDoesNotExist()
        {
            return createResponse(Status::CODE_404, "Song not found");
        }


        const long m_dataSize = std::numeric_limits<long long int>::max();
    };
}
#endif
