#ifndef YEARMANAGER_H_
#define YEARMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class yearManager
    {
    public:
        yearManager(const Model::BinaryPath&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
