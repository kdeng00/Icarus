#ifndef COVERARTMANAGER_H_
#define COVERARTMANAGER_H_

#include <string>
#include <utility>

#include "icarus_lib/icarus.h"

#include "model/Models.h"

namespace manager {
    class CoverArtManager {
    public:
        CoverArtManager(const icarus_lib::binary_path & bConf);

        model::Cover saveCover(const model::Song&);

        std::pair<bool, std::string> defaultCover(const model::Cover&);
        
        void deleteCover(const model::Song&);
        void updateCover(const model::Song&, const model::Song&);
        void updateCoverRecord(const model::Song&);
    private:
        std::string createImagePath(const model::Song&);

        icarus_lib::binary_path  m_bConf;
    };
}

#endif
