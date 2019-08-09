#include <iostream>
#include <string>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <string.h>

#include "metadata_retriever.h"

extern "C"
{

void retrieve_metadata(Song*, char*);

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

}
