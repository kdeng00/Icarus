#ifndef BASEMANAGER_H_
#define BASEMANAGER_H_

#include "icarus_lib/icarus.h"

namespace manager
{
    template<typename config_detail>
    class BaseManager
    {
    protected:
        BaseManager(const config_detail &config) :
            m_config(config)
        {
        }

        config_detail m_config;
    private:
    };
}


#endif