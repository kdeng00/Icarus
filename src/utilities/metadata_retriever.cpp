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

#include "managers/directory_manager.h"
#include "utilities/imageFile.h"
#include "utilities/metadata_retriever.h"

namespace fs = std::filesystem;

Song metadata_retriever::retrieve_metadata(std::string& song_path)
{
    TagLib::FileRef file(song_path.c_str());
    Song song;
    song.title = file.tag()->title().toCString();
    song.artist = file.tag()->artist().toCString();
    song.album = file.tag()->album().toCString();
    song.genre = file.tag()->genre().toCString();
    song.year = file.tag()->year();
    song.track = file.tag()->track();
    song.duration = file.audioProperties()->lengthInSeconds();
    song.songPath = song_path;

    // TODO: Move over to this eventually since at the moment
    // I am only targetting mp3 files
    // Used to retrieve disc
    TagLib::MPEG::File sameFile(song_path.c_str());
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

Cover metadata_retriever::update_cover_art(const Song& song, Cover& cov, const std::string& stockCoverPath)
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

        imageFile stock_img(cov.imagePath.c_str());

        TagLib::ID3v2::AttachedPictureFrame *pic = 
            new TagLib::ID3v2::AttachedPictureFrame;
        pic->setPicture(stock_img.data());
        pic->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);

        tag->addFrame(pic);

        sngF.save();
        std::cout << "applied stock cover art" << std::endl;
    } else {
        auto frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(
                frameList.front());
        directory_manager dir;
        auto img_path = dir.create_directory_process(song, cov.imagePath);
        img_path.append(song.title);
        img_path.append(".png");
        cov.imagePath = img_path;

        std::cout << cov.imagePath << std::endl;

        std::fstream img_save(cov.imagePath, std::ios::out | 
                std::ios::binary);
        img_save.write(frame->picture().data(), frame->picture().size());
        img_save.close();
        std::cout << "saved to " << cov.imagePath << std::endl;
    }

    return cov;
}

void metadata_retriever::update_metadata(Song sng_updated, const Song sng_old)
{
    std::cout<<"updating metadata"<<std::endl;
    TagLib::FileRef file(sng_old.songPath.c_str());
    
    if (sng_updated.title.size() > 0) {
        file.tag()->setTitle(sng_updated.title);
    }
    if (sng_updated.artist.size() > 0) {
        file.tag()->setArtist(sng_updated.artist);
    }
    if (sng_updated.album.size() > 0) {
        file.tag()->setAlbum(sng_updated.album);
    }
    if (sng_updated.genre.size() > 0) {
        file.tag()->setGenre(sng_updated.genre);
    }
    if (sng_updated.year > 0) {
        file.tag()->setYear(sng_updated.year);
    }

    // TODO: functionality to update the track number and disc number

    file.save();
}
