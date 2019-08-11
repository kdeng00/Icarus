#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

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

    cov.seekg(0);

    std::stringstream buf;
    std::copy(std::istreambuf_iterator<char>(cov),
        std::istreambuf_iterator<char>(),
        std::ostreambuf_iterator<char>(buf));

    return buf.str();
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
void copy_song_to_path(const char *target, const char *source)
{
    std::cout<<"starting process to copy song"<<std::endl;
    auto target_path = fs::path(target);
    auto src_path = fs::path(source);

    std::cout<<"copting over to "<<target_path.string()<<std::endl;
    fs::copy(src_path, target_path);

    fs::remove(source);
    std::cout<<"copy finished"<<std::endl;
}
void delete_cover_art_file(const std::string cov_path)
{
    auto cov = fs::path(cov_path);
    fs::remove(cov);
}
void delete_directories(Song song, const char *root_path)
{
    std::cout<<"checking to for empty directories to delete"<<std::endl;
    const std::string art{root_path + std::string{"/"} + song.Artist};
    const std::string alb{art + "/" + song.Album};

    auto alb_path = fs::path(alb);
    
    if (!fs::exists(alb_path)) {
        std::cout<<"directory does not exists"<<std::endl;
    } else if (fs::is_empty(alb_path)) {
        fs::remove(alb_path);
    }

    auto art_path = fs::path(art);
    
    if (!fs::exists(art_path)) {
        std::cout<<"directory does not exists"<<std::endl;
        return;
    } else if (fs::is_empty(art_path)) {
        fs::remove(art_path);
    }

    std::cout<<"deleted empty directory or directories"<<std::endl;
}
void delete_song(Song song)
{
    std::cout<<"deleting song"<<std::endl;
    auto song_path = fs::path(song.SongPath);

    if (!fs::exists(song_path)) {
        std::cout<<"song does not exists"<<std::endl;
        return;
    }

    fs::remove(song_path);
    std::cout<<"deleted song"<<std::endl;
}

extern "C"
{

void create_directory(Song, const char*, char*);
void copy_stock_cover_art(const char*, const char*);
void copy_song(const char*, const char*);
void delete_cover_art(const char*, const char*);
void delete_empty_directories(Song, const char*);
void delete_song_empty_directories(Song, const char*);
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
void copy_song(const char *target, const char *source)
{
    copy_song_to_path(target, source);
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
void delete_empty_directories(Song song, const char *root_path)
{
    delete_directories(song, root_path);
}
void delete_song_empty_directories(Song song, const char *root_path)
{
    delete_song(song);
    delete_directories(song, root_path);
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
