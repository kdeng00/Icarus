#ifndef ARTISTMANAGER_H_
#define ARTISTMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class artistManager
    {
    public:
        artistManager(const Model::BinaryPath&);

        Model::Artist retrieveArtist(Model::Artist&);
        Model::Artist saveArtist(const Model::Song&);

        static void printArtist(const Model::Artist&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
