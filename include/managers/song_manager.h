#ifndef SONGMANAGER_H_
#define SONGMANAGER_H_

#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "models/models.h"

class song_manager
{
public:
    song_manager(std::string&);

    /**
    std::string retrieveMusicPath();
    std::string retrieveTempPath();
    */

    //nlohmann::json pathConfigContent();

    void saveSong(Song&);

    static void printSong(const Song&);
private:
    void saveSongTemp(Song&);

    std::string exe_path;
};

#endif
