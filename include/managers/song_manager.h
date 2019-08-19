#ifndef SONGMANAGER_H_
#define SONGMANAGER_H_

#include <iostream>
#include <string>

#include "models.h"

class song_manager
{
public:
    song_manager(std::string&);

    void saveSong(Song&);
private:
    std::string exe_path;
};

#endif
