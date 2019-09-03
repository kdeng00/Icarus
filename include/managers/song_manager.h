#ifndef SONGMANAGER_H_
#define SONGMANAGER_H_

#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "models/models.h"

namespace Manager
{
    class song_manager
    {
    public:
        song_manager(std::string&);
        song_manager(const Model::BinaryPath&);

        void saveSong(Model::Song&);
        void deleteSong(Model::Song&);

        static void printSong(const Model::Song&);
    private:
        void saveSongTemp(Model::Song&);
        void saveMisc(Model::Song&);

        Model::BinaryPath m_bConf;
        std::string exe_path;
    };
}

#endif
