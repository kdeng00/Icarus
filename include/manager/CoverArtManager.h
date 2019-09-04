#ifndef COVERARTMANAGER_H_
#define COVERARTMANAGER_H_

#include <string>

#include "model/Models.h"

namespace manager
{
    class CoverArtManager
    {
    public:
        CoverArtManager(const std::string&);
        CoverArtManager(const model::BinaryPath& bConf);

        model::Cover saveCover(const model::Song&, std::string&, const std::string&);
    private:
        model::BinaryPath m_bConf;
        std::string path;
    };
}

#endif
