#ifndef GENREMANAGER_H_
#define GENREMANAGER_H_

#include "icarus_lib/icarus.h"

namespace manager {
    class GenreManager {
    public:
        GenreManager(const icarus_lib::binary_path &);

        icarus_lib::genre retrieveGenre(icarus_lib::genre&);
        icarus_lib::genre saveGenre(const icarus_lib::song&);

        void deleteGenre(const icarus_lib::song&);
        void updateGenre(icarus_lib::song&, const icarus_lib::song&);

        static void printGenre(const icarus_lib::genre&);
    private:
        icarus_lib::binary_path  m_bConf;
    };
}

#endif
