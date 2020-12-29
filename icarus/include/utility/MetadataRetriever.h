#ifndef METADATA_RETRIEVER_H_
#define METADATA_RETRIEVER_H_

#include <iostream>
#include <string>

#include "icarus_lib/icarus.h"
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>

namespace utility {
    class MetadataRetriever {
    public:
        icarus_lib::song retrieveMetadata(icarus_lib::song&);
        
        icarus_lib::cover updateCoverArt(const icarus_lib::song&, icarus_lib::cover&, const std::string&);
        icarus_lib::cover applyStockCoverArt(const icarus_lib::song&, icarus_lib::cover&, const std::string&);
        icarus_lib::cover applyCoverArt(const icarus_lib::song&, icarus_lib::cover&);

        bool songContainsCoverArt(const icarus_lib::song&);

        void updateMetadata(icarus_lib::song&, const icarus_lib::song&);
    private:
    };
}

#endif
