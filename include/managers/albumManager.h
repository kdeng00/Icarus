#ifndef ALBUMMANAGER_H_
#define ALBUMMANAGER_H_

#include "models/models.h"

namespace Manager
{
    class albumManager
    {
    public:
        albumManager(const Model::BinaryPath&);
    private:
        Model::BinaryPath m_bConf;
    };
}

#endif
