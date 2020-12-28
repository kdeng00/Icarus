#ifndef ARTISTMANAGER_H_
#define ARTISTMANAGER_H_

#include "model/Models.h"

namespace manager {
    class ArtistManager {
    public:
        ArtistManager(const model::BinaryPath&);

        model::Artist retrieveArtist(model::Artist&);
        model::Artist saveArtist(const model::Song&);

        void deleteArtist(const model::Song&);
        void updateArtist(model::Song&, const model::Song&);

        static void printArtist(const model::Artist&);
    private:
        model::BinaryPath m_bConf;
    };
}

#endif
