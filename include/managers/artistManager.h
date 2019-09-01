#ifndef ARTISTMANAGER_H_
#define ARTISTMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class artistManager
    {
    public:
        artistManager(const Model::BinaryPath&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
