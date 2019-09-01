#ifndef ALBUMMANAGER_H_
#define ALBUMMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class albumManager
    {
    public:
        albumManager(const Model::BinaryPath&);

        void saveAlbum(const Model::Song&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
