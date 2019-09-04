#ifndef SONGCONTROLLER_H_
#define SONGCONTROLLER_H_

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

#include "database/SongRepository.h"
#include "dto/SongDto.hpp"
#include "manager/SongManager.h"
#include "manager/TokenManager.h"
#include "model/Models.h"
#include "type/Scopes.h"
#include "type/SongFilter.h"

namespace fs = std::filesystem;

namespace controller
{
    class SongController : public oatpp::web::server::api::ApiController
    {
    public:
        SongController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper), m_exe_path(p)
        { }

        SongController(const model::BinaryPath& bConf, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
            : oatpp::web::server::api::ApiController(objectMapper), m_bConf(bConf)
        { }

        #include OATPP_CODEGEN_BEGIN(ApiController)

        // endpoint for uploading a song
        ENDPOINT("POST", "/api/v1/song/data", songUpload, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
            auto authHeader = request->getHeader("Authorization");

            OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

            auto auth = authHeader->std_str();
        
            manager::TokenManager tok;
            OATPP_ASSERT_HTTP(tok.isTokenValid(auth, type::Scope::upload), Status::CODE_403, "Not allowed");

            auto mp = std::make_shared<oatpp::web::mime::multipart::Multipart>(request->getHeaders());

            oatpp::web::mime::multipart::Reader mp_reader(mp.get());

            mp_reader.setPartReader("file", oatpp::web::mime::multipart::createInMemoryPartReader(m_dataSize));

            request->transferBody(&mp_reader);

            auto file = mp->getNamedPart("file");

            OATPP_ASSERT_HTTP(file, Status::CODE_400, "file is null");

            auto buff = std::unique_ptr<char>(new char[file->getKnownSize()]);
            auto buffSize = file->getInputStream()->read(buff.get(), file->getKnownSize());

            std::vector<unsigned char> data(buff.get(), buff.get() + buffSize);

            model::Song sng;
            sng.data = std::move(data);
        
            manager::SongManager songMgr(m_bConf);
            songMgr.saveSong(sng);

            return createResponse(Status::CODE_200, "OK");
        }

        // endpoint for retrieving all song records in json format
        ENDPOINT("GET", "/api/v1/song", songRecords, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
        {
            std::cout << "starting process of retrieving songs" << std::endl;
            database::SongRepository songRepo(m_bConf);
            auto songsDb = songRepo.retrieveRecords();
            auto songs = oatpp::data::mapping::type::List<dto::SongDto::ObjectWrapper>::createShared();

            std::cout << "creating object to send" << std::endl;
            for (auto& songDb : songsDb) {
                auto song = dto::SongDto::createShared();
                song->id = songDb.id;
                song->title = songDb.title.c_str();
                song->artist = songDb.artist.c_str();
                song->album = songDb.album.c_str();
                song->genre = songDb.genre.c_str();
                song->year = songDb.year;
                song->duration = songDb.duration;
                song->track = songDb.track;
                song->disc = songDb.disc;

                songs->pushBack(song);
            }

            return createDtoResponse(Status::CODE_200, songs);
        }

        // endpoint for retrieving song record by the song id in json format
        ENDPOINT("GET", "/api/v1/song/{id}", songRecord, 
            PATH(Int32, id)) {

            database::SongRepository songRepo(m_bConf);
            model::Song songDb;
            songDb.id = id;

            OATPP_ASSERT_HTTP(songRepo.doesSongExist(songDb, type::SongFilter::id) , Status::CODE_403, "song does not exist");

            std::cout << "song exists" << std::endl;
            songDb = songRepo.retrieveRecord(songDb, type::SongFilter::id);

            auto song = dto::SongDto::createShared();
            song->id = songDb.id;
            song->title = songDb.title.c_str();
            song->artist = songDb.artist.c_str();
            song->album = songDb.album.c_str();
            song->genre = songDb.genre.c_str();
            song->year = songDb.year;
            song->duration = songDb.duration;
            song->track = songDb.track;
            song->disc = songDb.disc;

            return createDtoResponse(Status::CODE_200, song);
        }

        ENDPOINT("GET", "/api/v1/song/data/{id}", downloadSong, 
            PATH(Int32, id)) {

            database::SongRepository songRepo(m_bConf);
            model::Song songDb;
            songDb.id = id;
            songDb = songRepo.retrieveRecord(songDb, type::SongFilter::id);

            std::ifstream fl(songDb.songPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);
            fl.seekg(0);

            std::stringstream buf;
            std::copy(std::istreambuf_iterator<char>(fl),
                std::istreambuf_iterator<char>(),
                std::ostreambuf_iterator<char>(buf));
            fl.close();

            auto rawSong = std::make_shared<oatpp::String>(oatpp::String(buf.str().data(), (v_int32)buf.str().size(), true));
        
            auto response = createResponse(Status::CODE_200, *rawSong);
            response->putHeader(Header::CONTENT_TYPE, "audio/mpeg");

            return response;
        }

        // TODO: create endpoint for updating songs

        ENDPOINT("DELETE", "api/v1/song/data/{id}", songDelete, PATH(Int32, id)) {

            model::Song song;
            song.id = id;

            manager::SongManager sngMgr(m_bConf);
            sngMgr.deleteSong(song);

            return createResponse(Status::CODE_200, "OK");
        }

        // TODO: create endpoint for streaming songs

        #include OATPP_CODEGEN_END(ApiController)
    private:
        std::string m_exe_path;
        model::BinaryPath m_bConf;
        const long m_dataSize = std::numeric_limits<long long int>::max();
    };
}
#endif
