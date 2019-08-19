#include "managers/song_manager.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

song_manager::song_manager(std::string& x_path)
    : exe_path(x_path)
{ }

void song_manager::saveSong(Song& song)
{
    constexpr auto tmp_song = "/tmp/song.mp3";
    if (fs::exists(fs::path(tmp_song))) {
        std::cout << "deleting old song " << std::endl;
        fs::remove(fs::path(tmp_song));
    }
    std::fstream s(tmp_song, std::fstream::binary | std::fstream::out);
    s.write((char*)&song.data[0], song.data.size());
    s.close();
}
