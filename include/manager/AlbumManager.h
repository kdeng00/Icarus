#ifndef ALBUMMANAGER_H_
#define ALBUMMANAGER_H_

#include "model/Models.h"

namespace manager
{
    class AlbumManager
    {
    public:
        AlbumManager(const model::BinaryPath&);

        model::Album retrieveAlbum(model::Album&);
        model::Album saveAlbum(const model::Song&);

        void deleteAlbum(const model::Song&);

        static void printAlbum(const model::Album&);
    private:
        model::BinaryPath m_bConf;
    };
}

#endif
