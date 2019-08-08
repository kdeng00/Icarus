#include <iostream>
#include <string>
#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <string.h>

#include "metadata_retriever.h"

void fetch_metadata(Song &sng, const char *song_path)
{
    //Song sng;
    std::cout<<"extracting metadata from: "<<std::endl;
    std::cout<<song_path<<std::endl;

    TagLib::FileRef file(song_path);
    //std::cout<<"0"<<std::endl;
    sng.Title = (char*)file.tag()->title().toCString();
    //sng->Title = (char*)file.tag()->title().toCString();
    std::cout<<"Title: "<<sng.Title<<std::endl;
    //std::cout<<"Title: "<<sng->Title<<std::endl;
    //std::cout<<"0"<<std::endl;
    sng.Artist = (char*)file.tag()->artist().toCString();
    //sng->Artist = (char*)file.tag()->artist().toCString();
    //std::cout<<"0"<<std::endl;
    sng.Album = (char*)file.tag()->album().toCString();
    //sng->Album = (char*)file.tag()->album().toCString();
    //std::cout<<"0"<<std::endl;
    sng.Genre = (char*)file.tag()->genre().toCString();
    //sng->Genre = (char*)file.tag()->genre().toCString();
    //std::cout<<"0"<<std::endl;
    sng.Year = file.tag()->year();
    //sng->Year = file.tag()->year();
    //std::cout<<"0"<<std::endl;
    sng.SongPath = (char*)song_path;
    //sng->SongPath = (char*)song_path;
    //std::cout<<"0"<<std::endl;
}

extern "C"
{

void retrieve_metadata(Song*, char*);

void retrieve_metadata(Song *song, char* song_path)
{
    std::cout<<"extern C"<<std::endl;
    //Song sng;
    Song *sng;
    //fetch_metadata(sn,song_path);
    //
    //
    TagLib::FileRef file(song_path);
    //std::cout<<"0"<<std::endl;
    //sng.Title = (char*)file.tag()->title().toCString();
    sng->Title = (char*)file.tag()->title().toCString();
    //strcpy(sng->Title, file.tag()->title().toCString());
    //std::cout<<"Title: "<<sng.Title<<std::endl;
    std::cout<<"Title: "<<sng->Title<<std::endl;
    //std::cout<<"0"<<std::endl;
    //sng.Artist = (char*)file.tag()->artist().toCString();
    sng->Artist = (char*)file.tag()->artist().toCString();
    //std::cout<<"0"<<std::endl;
    //sng.Album = (char*)file.tag()->album().toCString();
    sng->Album = (char*)file.tag()->album().toCString();
    //std::cout<<"0"<<std::endl;
    //sng.Genre = (char*)file.tag()->genre().toCString();
    sng->Genre = (char*)file.tag()->genre().toCString();
    //std::cout<<"0"<<std::endl;
    //sng.Year = file.tag()->year();
    sng->Year = file.tag()->year();
    //std::cout<<"0"<<std::endl;
    //sng.SongPath = (char*)song_path;
    sng->SongPath = (char*)song_path;
    //std::cout<<"res"<<std::endl<<sn.Title<<std::endl;
    //*song = sn;
    std::cout<<"done"<<std::endl;
}

}
