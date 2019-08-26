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
        : oatpp::web::server::api::ApiController(objectMapper), exe_path(p)
    { }

    #include OATPP_CODEGEN_BEGIN(ApiController)

    // endpoint for uploading a song
    ENDPOINT("POST", "/api/v1/song/data", songUpload, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        auto authHeader = request->getHeader("Authorization");

        OATPP_ASSERT_HTTP(authHeader, Status::CODE_403, "Nope");

        auto auth = authHeader->std_str();
        
        token_manager tok;
        OATPP_ASSERT_HTTP(tok.is_token_valid(auth, Scope::upload), Status::CODE_403, "Not allowed");

        auto mp = std::make_shared<oatpp::web::mime::multipart::Multipart>(request->getHeaders());

        oatpp::web::mime::multipart::Reader mp_reader(mp.get());

        mp_reader.setPartReader("file", oatpp::web::mime::multipart::createInMemoryPartReader(dataSize));

        request->transferBody(&mp_reader);

        auto file = mp->getNamedPart("file");

        OATPP_ASSERT_HTTP(file, Status::CODE_400, "file is null");

        auto stream = file->getInputStream();
        auto buff = std::unique_ptr<char>(new char[file->getKnownSize()]);
        auto buffSize = stream->read(buff.get(), file->getKnownSize());

        std::vector<unsigned char> data(buff.get(), buff.get() + buffSize);

        Song sng;
        sng.data = std::move(data);
        
        song_manager s_mgr(exe_path);
        s_mgr.saveSong(sng);

        return createResponse(Status::CODE_200, "OK");
    }

    // endpoint for retrieving all song records in json format
    ENDPOINT("GET", "/api/v1/song", songRecords, 
            REQUEST(std::shared_ptr<IncomingRequest>, request))
    {
        std::cout << "starting process of retrieving songs" << std::endl;
        songRepository songRepo(exe_path);
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

            songs->pushBack(song);
        }

        return createDtoResponse(Status::CODE_200, songs);
    }

    // endpoint for retrieving song record by the song id in json format
    ENDPOINT("GET", "/api/v1/song/{id}", songRecord, 
            PATH(Int32, id)) {

        songRepository songRepo(exe_path);
        Song songDb;
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

        return createDtoResponse(Status::CODE_200, song);
    }

    // TODO: left off here
    ENDPOINT("GET", "/api/v1/song/data/{id}", downloadSong, 
            PATH(Int32, id)) {

        songRepository songRepo(exe_path);
        Song songDb;
        songDb.id = id;
        songDb = songRepo.retrieveRecord(songDb, songFilter::id);

        std::cout << "song path " << songDb.songPath << std::endl;
        
        std::ifstream fl(songDb.songPath.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

        //auto rawSong = oatpp::String((v_int32) fl.tellg());
        fl.seekg(0);
        //fl.read((char*)rawSong->getData(), rawSong->getSize());

        std::stringstream buf;
        std::copy(std::istreambuf_iterator<char>(fl),
            std::istreambuf_iterator<char>(),
            std::ostreambuf_iterator<char>(buf)
            );
        fl.close();

        //std::cout << buf.str().c_str() << std::endl;

        //auto raw = buf.str();
       
        auto rawSong = std::make_shared<oatpp::String>(oatpp::String(buf.str().data(), (v_int32)buf.str().size(), true));
        //std::cout << rawSong->c_str() << std::endl;
        //std::cout << rawSong->std_str() << std::endl;
        //std::shared_ptr<oatpp::String> rawSong = std::make_shared<oatpp::String>();
        //rawSong->loadFromFile(songDb.songPath.c_str());

        /**
        std::cout << "constructing FileInputStream" << std::endl;
        oatpp::data::stream::FileInputStream file(songDb.songPath.c_str());
        //oatpp::data::stream::FileOutputStream file(songDb.songPath.c_str());
        auto songPath = fs::path(songDb.songPath);
        auto songSize = fs::file_size(songPath);
        std::cout << "prepping data" << std::endl;
        std::cout << "byte size " << songSize << std::endl;
        auto data = new char[songSize];
        std::cout << "data will be loaded" << std::endl;
        auto byteCount = file.read(&data, songSize);
        //auto byteCount = file.write(&data, songSize);

        std::cout << byteCount << " bytes read" << std::endl;

        auto songData = oatpp::data::stream::ChunkedBuffer::createShared();
        songData->write(data, byteCount);

        delete[] data;

        std::cout << "sending file" << std::endl;
        */
        /**
        auto songData = oatpp::data::stream::ChunkedBuffer::createShared();
        songData->write(buf.str().data(), buf.str().size());

        //auto response = createResponse(Status::CODE_200, songData);
        */
        auto response = createResponse(Status::CODE_200, *rawSong);
        response->putHeader("Accept-Ranges", "bytes");
        response->putHeader(Header::CONNECTION, Header::Value::CONNECTION_KEEP_ALIVE);
        response->putHeader(Header::CONTENT_TYPE, "audio/mpeg");

        return response;
    }

    #include OATPP_CODEGEN_END(ApiController)
private:
    std::string exe_path;
    const long dataSize = std::numeric_limits<long long int>::max();
};

#endif
