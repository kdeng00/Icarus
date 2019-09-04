#ifndef SONGMANAGER_H_
#define SONGMANAGER_H_

#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "model/Models.h"

namespace manager
{
    class SongManager
    {
    public:
        SongManager(std::string&);
        SongManager(const model::BinaryPath&);

        void saveSong(model::Song&);
        void deleteSong(model::Song&);

        static void printSong(const model::Song&);
    private:
        void saveSongTemp(model::Song&);
        void saveMisc(model::Song&);

        model::BinaryPath m_bConf;
        std::string exe_path;
    };
}

#endif
