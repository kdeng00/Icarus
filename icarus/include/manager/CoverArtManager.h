#ifndef COVERARTMANAGER_H_
#define COVERARTMANAGER_H_

#include <string>
#include <utility>

#include "icarus_lib/icarus.h"

namespace manager {
    class CoverArtManager {
    public:
        CoverArtManager(const icarus_lib::binary_path & bConf);

        icarus_lib::cover saveCover(const icarus_lib::song&);

        std::pair<bool, std::string> defaultCover(const icarus_lib::cover&);
        
        void deleteCover(const icarus_lib::song&);
        void updateCover(const icarus_lib::song&, const icarus_lib::song&);
        void updateCoverRecord(const icarus_lib::song&);
    private:
        std::string createImagePath(const icarus_lib::song&);

        icarus_lib::binary_path  m_bConf;
    };
}

#endif
