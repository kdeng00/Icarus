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

#include "database/songRepository.h"
#include "../dto/songDto.hpp"
#include "managers/song_manager.h"
#include "managers/token_manager.h"
#include "models/models.h"
#include "types/scopes.h"
#include "types/songFilter.h"

namespace fs = std::filesystem;

class songController : public oatpp::web::server::api::ApiController
{
public:
    songController(std::string p, OATPP_COMPONENT(std::shared_ptr<ObjectMapper>, objectMapper))
        : oatpp::web::server::api::ApiController(objectMapper), m_exe_path(p)
    { }

    #include OATPP_CODEGEN_BEGIN(ApiController)

    // endpoint for uploading a song
    ENDPOINT("POST", "/api/v1/song/data", songUpload, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        auto authHeader = request->getHeader("Authorization");

        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

        auto auth = authHeader->std_str();
        
        Manager::token_manager tok;
        OATPP_ASSERT_HTTP(tok.is_token_valid(auth, Scope::upload), Status::CODE_403, "Not allowed");

        auto mp = std::make_shared<oatpp::web::mime::multipart::Multipart>(request->getHeaders());

        oatpp::web::mime::multipart::Reader mp_reader(mp.get());

        mp_reader.setPartReader("file", oatpp::web::mime::multipart::createInMemoryPartReader(m_dataSize));

        request->transferBody(&mp_reader);

        auto file = mp->getNamedPart("file");

        OATPP_ASSERT_HTTP(file, Status::CODE_400, "file is null");

        auto buff = std::unique_ptr<char>(new char[file->getKnownSize()]);
        auto buffSize = file->getInputStream()->read(buff.get(), file->getKnownSize());

        std::vector<unsigned char> data(buff.get(), buff.get() + buffSize);

        Model::Song sng;
        sng.data = std::move(data);
        
        Manager::song_manager s_mgr(m_exe_path);
        s_mgr.saveSong(sng);

        return createResponse(Status::CODE_200, "OK");
    }

    // endpoint for retrieving all song records in json format
    ENDPOINT("GET", "/api/v1/song", songRecords, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        std::cout << "starting process of retrieving songs" << std::endl;
        Database::songRepository songRepo(m_exe_path);
        auto songsDb = songRepo.retrieveRecords();
        auto songs = oatpp::data::mapping::type::List<songDto::ObjectWrapper>::createShared();

        std::cout << "creating object to send" << std::endl;
        for (auto& songDb : songsDb) {
            auto song = songDto::createShared();
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

        Database::songRepository songRepo(m_exe_path);
        Model::Song songDb;
        songDb.id = id;

        songDb = songRepo.retrieveRecord(songDb, songFilter::id);

        auto song = songDto::createShared();
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

        Database::songRepository songRepo(m_exe_path);
        Model::Song songDb;
        songDb.id = id;
        songDb = songRepo.retrieveRecord(songDb, songFilter::id);

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

        Model::Song song;
        song.id = id;

        Manager::song_manager sngMgr(m_exe_path);
        sngMgr.deleteSong(song);

        return createResponse(Status::CODE_200, "OK");
    }

    // TODO: create endpoint for streaming songs

    #include OATPP_CODEGEN_END(ApiController)
private:
    std::string m_exe_path;
    const long m_dataSize = std::numeric_limits<long long int>::max();
};

#endif
