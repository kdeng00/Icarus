#ifndef YEARMANAGER_H_
#define YEARMANAGER_H_

#include "model/Models.h"

namespace manager {
    class YearManager {
    public:
        YearManager(const model::BinaryPath&);

        model::Year retrieveYear(model::Year&);
        model::Year saveYear(const model::Song&);

        void deleteYear(const model::Song&);
        void updateYear(model::Song&, const model::Song&);

        static void printYear(const model::Year&);
    private:
        model::BinaryPath m_bConf;
    };
}

#endif
