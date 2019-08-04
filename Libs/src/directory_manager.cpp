#include <iostream>
#include <filesystem>
#include <fstream>
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

std::string read_cover_art(const char *source)
{
    auto source_path = fs::path(source);
    
    std::fstream cov(source, std::ios::in | std::ios::binary);

    if (!cov.is_open()) {
        std::cout<<"file is not open"<<std::endl;
    }

    cov.seekg(0, cov.end);
    int cov_len = cov.tellg();
    cov.seekg(0, cov.beg);

    char buff[cov_len];
    cov.read(buff, cov_len);
    cov.close();

    return std::string{buff};
}
void copy_stock_to_root(const char *target, const std::string buff)
{
    std::cout<<"starting process"<<std::endl;
    auto target_path = fs::path(target);
    if (fs::exists(target_path)) {
        std::cout<<target_path.string()<<" exists"<<std::endl;
        return;
    }

    std::cout<<target_path.string()<<" does not exist, copying over"<<std::endl;
    std::fstream cov(target, std::ios::out | std::ios::binary);
    cov.write(buff.c_str(), buff.size());
    cov.close();

    std::cout<<"copy finished"<<std::endl;
}
void delete_cover_art_file(const std::string cov_path)
{
    auto cov = fs::path(cov_path);
    fs::remove(cov);
}

extern "C"
{

void create_directory(Song, const char*, char*);
void copy_stock_cover_art(const char*, const char*);
void print_song_details(const Song);

void create_directory(Song song, const char *root_path, char *dir)
{
    const auto tmp = create_directory_process(song, root_path);
    size_t tmp_sz = tmp.size();
    tmp.copy(dir, tmp_sz, 0);
}
void copy_stock_cover_art(const char *target, const char *source)
{
    const auto buff = read_cover_art(source);
    copy_stock_to_root(target, buff);
}
void delete_cover_art(const char *cover_path, const char *stock_path)
{
    std::cout<<"starting process to delete cover art"<<std::endl;
    const auto cov_path = std::string{cover_path};
    const auto s_path = std::string{stock_path};
    if (cov_path.compare(s_path) != 0) {
        std::cout<<"cover art is not the stock path"<<std::endl;
        delete_cover_art_file(cov_path);
    } else {
        std::cout<<"cover art is the stock path and will not be deleted"<<std::endl;
    }
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
