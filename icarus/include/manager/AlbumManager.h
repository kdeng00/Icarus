#ifndef ALBUMMANAGER_H_
#define ALBUMMANAGER_H_

#include "icarus_lib/icarus.h"

namespace manager {
    class AlbumManager {
    public:
        AlbumManager(const icarus_lib::binary_path &);

        icarus_lib::album retrieveAlbum(icarus_lib::album&);
        icarus_lib::album saveAlbum(const icarus_lib::song&);

        void deleteAlbum(const icarus_lib::song&);
        void updateAlbum(icarus_lib::song&, const icarus_lib::song&);

        static void printAlbum(const icarus_lib::album&);
    private:
        icarus_lib::binary_path  m_bConf;
    };
}

#endif
