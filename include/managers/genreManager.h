#ifndef GENREMANAGER_H_
#define GENREMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class genreManager
    {
    public:
        genreManager(const Model::BinaryPath&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
