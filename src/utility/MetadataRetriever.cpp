#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <string.h>

#include <attachedpictureframe.h>
#include <fileref.h>
#include <mpegfile.h>
#include <tag.h>

#include "manager/DirectoryManager.h"
#include "utility/ImageFile.h"
#include "utility/MetadataRetriever.h"

namespace fs = std::filesystem;

model::Song utility::MetadataRetriever::retrieveMetadata(std::string& songPath)
{
    TagLib::FileRef file(songPath.c_str());
    model::Song song;
    song.title = file.tag()->title().toCString();
    song.artist = file.tag()->artist().toCString();
    song.album = file.tag()->album().toCString();
    song.genre = file.tag()->genre().toCString();
    song.year = file.tag()->year();
    song.track = file.tag()->track();
    song.duration = file.audioProperties()->lengthInSeconds();
    song.songPath = songPath;

    // TODO: Move over to this eventually since at the moment
    // I am only targetting mp3 files
    // Used to retrieve disc
    TagLib::MPEG::File sameFile(songPath.c_str());
    auto tag = sameFile.ID3v2Tag();

    auto frame = tag->frameList("TPOS");
    if (frame.isEmpty()) {
        song.disc = 1;
        // TODO: set default disc to 1 if none found
    } else {
        song.disc = std::stoi(frame.front()->toString().toCString());
    }


    return song;
}

model::Cover utility::MetadataRetriever::updateCoverArt(const model::Song& song, model::Cover& cov, const std::string& stockCoverPath)
{
    TagLib::MPEG::File sngF(song.songPath.c_str());
    auto tag = sngF.ID3v2Tag();
    auto frameList = tag->frameListMap()["APIC"];

    if (frameList.isEmpty()) {
        cov.imagePath.append("CoverArt.png");
        
        if (!fs::exists(cov.imagePath)) {
            std::cout << "copying stock cover path" << std::endl;
            fs::copy(stockCoverPath, cov.imagePath);
        }

        ImageFile stockImg(cov.imagePath.c_str());

        TagLib::ID3v2::AttachedPictureFrame *pic = 
            new TagLib::ID3v2::AttachedPictureFrame;
        pic->setPicture(stockImg.data());
        pic->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);

        tag->addFrame(pic);

        sngF.save();
        std::cout << "applied stock cover art" << std::endl;
    } else {
        auto frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(
                frameList.front());
        
        auto imgPath = manager::DirectoryManager::createDirectoryProcess(song, cov.imagePath);
        imgPath.append(song.title);
        imgPath.append(".png");
        cov.imagePath = imgPath;

        std::fstream imgSave(cov.imagePath, std::ios::out | 
                std::ios::binary);
        imgSave.write(frame->picture().data(), frame->picture().size());
        imgSave.close();
        std::cout << "saved to " << cov.imagePath << std::endl;
    }

    return cov;
}

void utility::MetadataRetriever::updateMetadata(model::Song& sngUpdated, const model::Song& sngOld)
{
    std::cout<<"updating metadata"<<std::endl;
    TagLib::FileRef file(sngOld.songPath.c_str());
    
    if (sngUpdated.title.size() > 0) {
        file.tag()->setTitle(sngUpdated.title);
    }
    if (sngUpdated.artist.size() > 0) {
        file.tag()->setArtist(sngUpdated.artist);
    }
    if (sngUpdated.album.size() > 0) {
        file.tag()->setAlbum(sngUpdated.album);
    }
    if (sngUpdated.genre.size() > 0) {
        file.tag()->setGenre(sngUpdated.genre);
    }
    if (sngUpdated.year > 0) {
        file.tag()->setYear(sngUpdated.year);
    }

    // TODO: functionality to update the track number and disc number

    file.save();
}
