#include <iostream>
#include <string>

#include "metadata_retriever.h"

Song fetch_metadata(const char *song_path)
{
    Song sng;
    std::string title = "ddd";
    sng.Title = (char*)title.c_str();
    sng.SongPath = (char*)song_path;

    return sng;
}

extern "C"
{

Song retrieve_metadata(const char*);

Song retrieve_metadata(const char* song_path)
{
    return fetch_metadata(song_path);
}

}
