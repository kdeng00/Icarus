#ifndef YEARMANAGER_H_
#define YEARMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class yearManager
    {
    public:
        yearManager(const Model::BinaryPath&);

        Model::Year retrieveYear(Model::Year&);
        Model::Year saveYear(const Model::Song&);

        static void printYear(const Model::Year&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
