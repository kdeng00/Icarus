#ifndef METADATA_RETRIEVER_H_
#define METADATA_RETRIEVER_H_

#include <iostream>
#include <string>

#include "model/Models.h"

namespace utility {
class MetadataRetriever
{
public:
    model::Song retrieveMetadata(std::string&);

    model::Cover updateCoverArt(const model::Song&, model::Cover&, const std::string&);
    model::Cover applyStockCoverArt(const model::Song&, model::Cover&, const std::string&);
    model::Cover applyCoverArt(const model::Song&, model::Cover&);

    bool songContainsCoverArt(const model::Song&);

    void updateMetadata(model::Song&, const model::Song&);
private:
};
}

#endif
