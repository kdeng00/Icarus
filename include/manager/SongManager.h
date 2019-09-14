#ifndef SONGMANAGER_H_
#define SONGMANAGER_H_

#include <iostream>
#include <string>

#include <nlohmann/json.hpp>

#include "dto/SongDto.hpp"
#include "model/Models.h"

namespace manager
{
    class SongManager
    {
    public:
        SongManager(std::string&);
        SongManager(const model::BinaryPath&);

        bool didSongChange(const model::Song&, const model::Song&);
        bool requiresFilesystemChange(const model::Song&, const model::Song&);

        void saveSong(model::Song&);
        void deleteSong(model::Song&);
        void updateSong(model::Song&);

        static model::Song songDtoConv(dto::SongDto::ObjectWrapper&);

        static void printSong(const model::Song&);
    private:
        void saveSongTemp(model::Song&);
        void saveMisc(model::Song&);
        void deleteMisc(const model::Song&);

        model::BinaryPath m_bConf;
        std::string exe_path;
    };
}

#endif
