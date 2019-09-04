#ifndef METADATA_RETRIEVER_H_
#define METADATA_RETRIEVER_H_

#include <iostream>
#include <string>

#include "model/Models.h"

namespace utility
{
    class MetadataRetriever
    {
    public:
        model::Song retrieveMetadata(std::string&);
        model::Cover updateCoverArt(const model::Song&, model::Cover&, const std::string&);

        void updateMetadata(model::Song&, const model::Song&);
    private:
    };
}

#endif
