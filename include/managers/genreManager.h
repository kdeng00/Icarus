#ifndef GENREMANAGER_H_
#define GENREMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class genreManager
    {
    public:
        genreManager(const Model::BinaryPath&);

        Model::Genre retrieveGenre(Model::Genre&);
        Model::Genre saveGenre(const Model::Song&);

        static void printGenre(const Model::Genre&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
