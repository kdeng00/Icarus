#ifndef GENREMANAGER_H_
#define GENREMANAGER_H_

#include "model/Models.h"

namespace manager
{
    class GenreManager
    {
    public:
        GenreManager(const model::BinaryPath&);

        model::Genre retrieveGenre(model::Genre&);
        model::Genre saveGenre(const model::Song&);

        void deleteGenre(const model::Song&);
        void updateGenre(model::Song&, const model::Song&);

        static void printGenre(const model::Genre&);
    private:
        model::BinaryPath m_bConf;
    };
}

#endif
