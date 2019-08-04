#include <iostream>
#include <filesystem>
#include <string>

#include "directory_manager.h"

namespace fs = std::filesystem;

std::string create_directory_process(Song song, const char *root_path)
{
    auto curr_path = fs::path(root_path);

    if (fs::exists(curr_path)) {
        std::cout<<"path exists"<<std::endl;
    } else {
        std::cout<<"creating path"<<std::endl;
        fs::create_directory(curr_path);
    }

    auto art_path = fs::path(curr_path.string() + song.Artist);
    if (fs::exists(art_path)) {
        std::cout<<"artist path exists"<<std::endl;
    } else {
        std::cout<<"creating artist path"<<std::endl;
        fs::create_directory(art_path);
    }

    auto alb_path = fs::path(art_path.string() + "/" + song.Album);
    if (fs::exists(alb_path)) {
        std::cout<<"album path exists"<<std::endl;
    } else {
        std::cout<<"creating album path"<<std::endl;
        fs::create_directory(alb_path);
    }

    return alb_path.string() + "/";
}

extern "C"
{

void create_directory(Song, const char*, char*);
void print_song_details(const Song);

void create_directory(Song song, const char *root_path, char *dir)
{
    std::cout<<"c++ creating directory"<<std::endl;

    std::string tmp = create_directory_process(song, root_path);
    std::cout<<"tmp size "<<tmp.size()<<std::endl;
    //dir = (char*)tmp.c_str();
    std::cout<<"done"<<std::endl;
    size_t tmp_sz = tmp.size();
    tmp.copy(dir, tmp_sz, 0);
    //std::copy(tmp.begin(), tmp.end()-1, dir);
    std::cout<<"still c++ "<<dir<<"\n\n";
}
void print_song_details(const Song song)
{
    std::cout<<"song details"<<std::endl;
    std::cout<<"title: "<<song.Title<<std::endl;
    std::cout<<"artist: "<<song.Artist<<std::endl;
    std::cout<<"album: "<<song.Album<<std::endl;
    std::cout<<"genre: "<<song.Genre<<std::endl;
    std::cout<<"year: "<<song.Year<<std::endl;
}

}
