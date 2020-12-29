#ifndef SONGMANAGER_H_
#define SONGMANAGER_H_

#include <iostream>
#include <map>
#include <string>
#include <utility>

#include "icarus_lib/icarus.h"
#include <nlohmann/json.hpp>

#include "dto/SongDto.hpp"
#include "type/SongChanged.h"
#include "type/SongUpload.h"

namespace manager {
    class SongManager {
    public:
        SongManager(const icarus_lib::binary_path &);

        std::pair<bool, type::SongUpload> saveSong(icarus_lib::song&);

        bool didSongChange(const icarus_lib::song&, const icarus_lib::song&);
        bool requiresFilesystemChange(const icarus_lib::song&, const icarus_lib::song&);
        bool deleteSong(icarus_lib::song&);
        bool updateSong(icarus_lib::song&);

        static void printSong(const icarus_lib::song&);
    private:
        std::map<type::SongChanged, bool> changesInSong(const icarus_lib::song&, const icarus_lib::song&);

        std::string createSongPath(const icarus_lib::song&);

        void assignMiscId(icarus_lib::song&, const icarus_lib::song&);
        void assignMiscFields(std::map<type::SongChanged, bool>&, icarus_lib::song&, 
            const icarus_lib::song&);
        void saveSongTemp(icarus_lib::song&);
        void saveMisc(icarus_lib::song&);
        void deleteMisc(const icarus_lib::song&);
        void deleteMiscExceptCoverArt(const icarus_lib::song&);
        void updateMisc(const std::map<type::SongChanged, bool>&, 
            icarus_lib::song&, const icarus_lib::song&);
        void modifySongOnFilesystem(icarus_lib::song&, const icarus_lib::song&);

        icarus_lib::binary_path  m_bConf;
    };
}

#endif
