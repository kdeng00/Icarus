#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <string.h>

#include <taglib/attachedpictureframe.h>
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>

#include "directory_manager.h"
#include "metadata_retriever.h"
#include "imageFile.h"

extern "C"
{

void retrieve_metadata(Song*, char*);
void update_metadata(Song*, Song*);

void retrieve_metadata(Song *sng, char* song_path)
{
    std::cout<<"extern C"<<std::endl;
    TagLib::FileRef file(song_path);

    strcpy(sng->Title, file.tag()->title().toCString());
    strcpy(sng->Artist, file.tag()->artist().toCString());
    strcpy(sng->Album, file.tag()->album().toCString());
    strcpy(sng->Genre, file.tag()->genre().toCString());
    sng->Year = file.tag()->year();
    sng->Duration = file.audioProperties()->lengthInSeconds();
    strcpy(sng->SongPath, song_path);
}
void update_metadata(Song *sng_updated, Song *sng_old)
{
    std::cout<<"updating metadata"<<std::endl;
    TagLib::FileRef file(sng_old->SongPath);
    
    if (strlen(sng_updated->Title) > 0) {
        file.tag()->setTitle(sng_updated->Title);
    }
    if (strlen(sng_updated->Artist) > 0) {
        file.tag()->setArtist(sng_updated->Artist);
    }
    if (strlen(sng_updated->Album) > 0) {
        file.tag()->setAlbum(sng_updated->Album);
    }
    if (strlen(sng_updated->Genre) > 0) {
        file.tag()->setGenre(sng_updated->Genre);
    }
    if (sng_updated->Year > 0) {
        file.tag()->setYear(sng_updated->Year);
    }

    file.save();
}
void update_cover_art(Cover *cov, Song *song, const char *root_path)
{
    TagLib::MPEG::File sngF(song->SongPath);
    TagLib::ID3v2::Tag *tag = sngF.ID3v2Tag();
    auto frameList = tag->frameListMap()["APIC"];

    if (frameList.isEmpty()) {
        std::string stock_path{root_path};
        stock_path.append("CoverArt.png");
        strcpy(cov->ImagePath, stock_path.c_str());

        imageFile stock_img(stock_path.c_str());

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
        auto img_path = create_directory_process(*song, root_path);
        img_path.append(song->Title);
        img_path.append(".png");
        strcpy(cov->ImagePath, img_path.c_str());
        std::cout<<cov->ImagePath<<std::endl;

        std::fstream img_save(cov->ImagePath, std::ios::out | 
                std::ios::binary);
        img_save.write(frame->picture().data(), frame->picture().size());
        img_save.close();
        std::cout<<"saved to "<<cov->ImagePath<<std::endl;
    }
}

}
