#ifndef GENREMANAGER_H_
#define GENREMANAGER_H_

#include "icarus_lib/icarus.h"

#include "model/Models.h"

namespace manager {
    class GenreManager {
    public:
        GenreManager(const icarus_lib::binary_path &);

        model::Genre retrieveGenre(model::Genre&);
        model::Genre saveGenre(const model::Song&);

        void deleteGenre(const model::Song&);
        void updateGenre(model::Song&, const model::Song&);

        static void printGenre(const model::Genre&);
    private:
        icarus_lib::binary_path  m_bConf;
    };
}

#endif
