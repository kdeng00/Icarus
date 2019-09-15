#ifndef SONGMANAGER_H_
#define SONGMANAGER_H_

#include <iostream>
#include <map>
#include <string>

#include <nlohmann/json.hpp>

#include "dto/SongDto.hpp"
#include "model/Models.h"
#include "type/SongChanged.h"

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
        std::map<type::SongChanged, bool> changesInSong(const model::Song&, const model::Song&);

        void assignMiscId(model::Song&, const model::Song&);
        void assignMiscFields(std::map<type::SongChanged, bool>&, model::Song&, 
            const model::Song&);
        void saveSongTemp(model::Song&);
        void saveMisc(model::Song&);
        void deleteMisc(const model::Song&);
        void updateMisc(const std::map<type::SongChanged, bool>&, 
            model::Song&, const model::Song&);
        void modifySongOnFilesystem(model::Song&, const model::Song&);

        model::BinaryPath m_bConf;
        std::string exe_path;
    };
}

#endif
