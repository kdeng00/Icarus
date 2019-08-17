#include "models.h"

#include <string>

std::string create_directory_process(Song, const char*);
std::string read_cover_art(const char*);

bool delete_song(Song*);

void copy_stock_to_root(const char*, const std::string);
void copy_song_to_path(const char*, const char*);
void delete_cover_art_file(const std::string);
void delete_directories(Song, const char*);
void delete_song(Song);
