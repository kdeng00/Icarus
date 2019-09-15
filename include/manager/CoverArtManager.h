#ifndef COVERARTMANAGER_H_
#define COVERARTMANAGER_H_

#include <string>
#include <utility>

#include "model/Models.h"

namespace manager
{
    class CoverArtManager
    {
    public:
        CoverArtManager(const std::string&);
        CoverArtManager(const model::BinaryPath& bConf);

        model::Cover saveCover(const model::Song&, std::string&, 
            const std::string&);

        std::pair<bool, std::string> defaultCover(const model::Cover&);
        
        void deleteCover(const model::Song&);
        void updateCover(const model::Song&);
        void updateCoverRecord(const model::Song&);
    private:
        model::BinaryPath m_bConf;
        std::string path;
    };
}

#endif
