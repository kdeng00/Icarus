#ifndef ALBUMMANAGER_H_
#define ALBUMMANAGER_H_

#include "icarus_lib/icarus.h"

#include "model/Models.h"

namespace manager {
    class AlbumManager {
    public:
        AlbumManager(const icarus_lib::binary_path &);

        model::Album retrieveAlbum(model::Album&);
        model::Album saveAlbum(const model::Song&);

        void deleteAlbum(const model::Song&);
        void updateAlbum(model::Song&, const model::Song&);

        static void printAlbum(const model::Album&);
    private:
        icarus_lib::binary_path  m_bConf;
    };
}

#endif
