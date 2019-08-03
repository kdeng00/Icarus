#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

constexpr auto app_setting = "appsettings.Development.json";

struct Song
{
    int Id;
    char *Title;
    char *Artist;
    char *Album;
    char *Genre;
    int Year;
};

void create_directory_process(Song);

void create_directory_process(Song song)
{
    auto curr_path = fs::current_path();
    std::cout<<"current path "<<curr_path.string()<<std::endl;
}

extern "C"
{

void create_directory(Song);
void print_song_details(const Song);

void create_directory(Song song)
{
    std::cout<<"c++ creating directory"<<std::endl;
    print_song_details(song);
    create_directory_process(song);
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
