#ifndef ARTISTMANAGER_H_
#define ARTISTMANAGER_H_

#include "icarus_lib/icarus.h"

#include "model/Models.h"

namespace manager {
    class ArtistManager {
    public:
        ArtistManager(const icarus_lib::binary_path &);

        model::Artist retrieveArtist(model::Artist&);
        model::Artist saveArtist(const model::Song&);

        void deleteArtist(const model::Song&);
        void updateArtist(model::Song&, const model::Song&);

        static void printArtist(const model::Artist&);
    private:
        icarus_lib::binary_path  m_bConf;
    };
}

#endif
