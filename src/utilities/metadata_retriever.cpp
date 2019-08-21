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

Song metadata_retriever::retrieve_metadata(std::string& song_path)
{
    TagLib::FileRef file(song_path.c_str());
    Song song;
    song.title = file.tag()->title().toCString();
    song.artist = file.tag()->artist().toCString();
    song.album = file.tag()->album().toCString();
    song.genre = file.tag()->genre().toCString();
    song.year = file.tag()->year();
    song.duration = file.audioProperties()->lengthInSeconds();
    song.songPath = song_path;

    /**
    strcpy(sng->Title, file.tag()->title().toCString());
    strcpy(sng->Artist, file.tag()->artist().toCString());
    strcpy(sng->Album, file.tag()->album().toCString());
    strcpy(sng->Genre, file.tag()->genre().toCString());
    sng->Year = file.tag()->year();
    sng->Duration = file.audioProperties()->lengthInSeconds();
    strcpy(sng->SongPath, song_path);
    */

    return song;
}

Cover metadata_retriever::update_cover_art(Cover cov, const Song song, const std::string& root_path)
{
    TagLib::MPEG::File sngF(song.songPath.c_str());
    TagLib::ID3v2::Tag *tag = sngF.ID3v2Tag();
    auto frameList = tag->frameListMap()["APIC"];

    if (frameList.isEmpty()) {
        std::string stock_path{root_path};
        stock_path.append("CoverArt.png");
        cov.imagePath = stock_path;
        //strcpy(cov->ImagePath, stock_path.c_str());

        imageFile stock_img(cov.imagePath.c_str());

        TagLib::ID3v2::AttachedPictureFrame *pic = 
            new TagLib::ID3v2::AttachedPictureFrame;
        pic->setPicture(stock_img.data());
        pic->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);

        tag->addFrame(pic);

        sngF.save();
        std::cout<<"applied stock cover art"<<std::endl;
    } else {
        auto frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(
                frameList.front());
        directory_manager dir;
        auto img_path = dir.create_directory_process(song, root_path);
        img_path.append(song.title);
        img_path.append(".png");
        cov.imagePath = img_path;
        //strcpy(cov->ImagePath, img_path.c_str());
        std::cout<<cov.imagePath<<std::endl;

        std::fstream img_save(cov.imagePath, std::ios::out | 
                std::ios::binary);
        img_save.write(frame->picture().data(), frame->picture().size());
        img_save.close();
        std::cout<<"saved to "<<cov.imagePath<<std::endl;
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

    file.save();
}
