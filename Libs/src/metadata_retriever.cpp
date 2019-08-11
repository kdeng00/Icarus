#include <iostream>
#include <string>
#include <string.h>

#include <taglib/tag.h>
#include <taglib/fileref.h>

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
void update_cover_art(Cover *cov, const char *stock_path, const char *song_path)
{
    const std::string img_frame = "APIC";
    TagLib::MPEG::File sngF(song_path);
    TagLib::ID3v2::Tag *tag = sngF.ID3v2Tag();
    auto frameList = tag->frameListMap()[img_frame.c_str()];

    if (frameList.isEmpty()) {
        // TODO: Apply stock cover art
        imageFile stock_img(stock_path);
    } else {
        // TODO: Extract cover art from song and store it in
        // cov->ImagePath
    }
}

}
