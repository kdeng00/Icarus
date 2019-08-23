#ifndef METADATA_RETRIEVER_H_
#define METADATA_RETRIEVER_H_

#include <iostream>
#include <string>

#include "models/models.h"

class metadata_retriever
{
public:
    Song retrieve_metadata(std::string&);
    Cover update_cover_art(const Song&, Cover& cov, const std::string&);

    void update_metadata(Song updated_song, const Song old_song);
private:
};

#endif
