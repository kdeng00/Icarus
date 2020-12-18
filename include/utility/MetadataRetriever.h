#ifndef METADATA_RETRIEVER_H_
#define METADATA_RETRIEVER_H_

#include <iostream>
#include <string>

// #include <attachedpictureframe.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/textidentificationframe.h>
#include <taglib/fileref.h>
#include <taglib/mpegfile.h>
#include <taglib/tag.h>

#include "model/Models.h"

namespace utility {
    class MetadataRetriever {
    public:
        model::Song retrieveMetadata(model::Song&);
        
        model::Cover updateCoverArt(const model::Song&, model::Cover&, const std::string&);
        model::Cover applyStockCoverArt(const model::Song&, model::Cover&, const std::string&);
        model::Cover applyCoverArt(const model::Song&, model::Cover&);

        bool songContainsCoverArt(const model::Song&);

        void updateMetadata(model::Song&, const model::Song&);
    private:
    };
}

#endif
