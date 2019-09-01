#ifndef COVERARTMANAGER_H_
#define COVERARTMANAGER_H_

#include <string>

#include "models/models.h"

namespace Manager
{
    class coverArtManager
    {
    public:
        coverArtManager(const std::string&);
        coverArtManager(const Model::BinaryPath& bConf);

        Model::Cover saveCover(const Model::Song&, std::string&, const std::string&);
    private:
        Model::BinaryPath m_bConf;
        std::string path;
    };
}

#endif
