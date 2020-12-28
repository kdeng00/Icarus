#ifndef YEARMANAGER_H_
#define YEARMANAGER_H_

#include "icarus_lib/icarus.h"

namespace manager {
    class YearManager {
    public:
        YearManager(const icarus_lib::binary_path &);

        icarus_lib::year retrieveYear(icarus_lib::year &);
        icarus_lib::year saveYear(const icarus_lib::song&);

        void deleteYear(const icarus_lib::song&);
        void updateYear(icarus_lib::song&, const icarus_lib::song&);

        static void printYear(const icarus_lib::year&);
    private:
        icarus_lib::binary_path m_bConf;
    };
}

#endif
