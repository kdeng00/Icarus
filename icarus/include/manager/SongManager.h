#ifndef SONGMANAGER_H_
#define SONGMANAGER_H_

#include <iostream>
#include <map>
#include <string>
#include <utility>

#include <nlohmann/json.hpp>

#include "dto/SongDto.hpp"
#include "model/Models.h"
#include "type/SongChanged.h"
#include "type/SongUpload.h"

namespace manager {
    class SongManager {
    public:
        SongManager(const model::BinaryPath&);

        std::pair<bool, type::SongUpload> saveSong(model::Song&);

        bool didSongChange(const model::Song&, const model::Song&);
        bool requiresFilesystemChange(const model::Song&, const model::Song&);
        bool deleteSong(model::Song&);
        bool updateSong(model::Song&);

        static void printSong(const model::Song&);
    private:
        std::map<type::SongChanged, bool> changesInSong(const model::Song&, const model::Song&);

        std::string createSongPath(const model::Song&);

        void assignMiscId(model::Song&, const model::Song&);
        void assignMiscFields(std::map<type::SongChanged, bool>&, model::Song&, 
            const model::Song&);
        void saveSongTemp(model::Song&);
        void saveMisc(model::Song&);
        void deleteMisc(const model::Song&);
        void deleteMiscExceptCoverArt(const model::Song&);
        void updateMisc(const std::map<type::SongChanged, bool>&, 
            model::Song&, const model::Song&);
        void modifySongOnFilesystem(model::Song&, const model::Song&);

        model::BinaryPath m_bConf;
    };
}

#endif
