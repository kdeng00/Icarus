#ifndef ALBUMMANAGER_H_
#define ALBUMMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class albumManager
    {
    public:
        albumManager(const Model::BinaryPath&);

        Model::Album retrieveAlbum(Model::Album&);
        Model::Album saveAlbum(const Model::Song&);

        static void printAlbum(const Model::Album&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
