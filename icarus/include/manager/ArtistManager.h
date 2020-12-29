#ifndef ARTISTMANAGER_H_
#define ARTISTMANAGER_H_

#include "icarus_lib/icarus.h"

namespace manager {
    class ArtistManager {
    public:
        ArtistManager(const icarus_lib::binary_path &);

        icarus_lib::artist retrieveArtist(icarus_lib::artist&);
        icarus_lib::artist saveArtist(const icarus_lib::song&);

        void deleteArtist(const icarus_lib::song&);
        void updateArtist(icarus_lib::song&, const icarus_lib::song&);

        static void printArtist(const icarus_lib::artist&);
    private:
        icarus_lib::binary_path  m_bConf;
    };
}

#endif
