#ifndef METADATA_RETRIEVER_H_
#define METADATA_RETRIEVER_H_

#include <iostream>
#include <string>

#include "models/models.h"

class metadata_retriever
{
public:
    Model::Song retrieve_metadata(std::string&);
    Model::Cover update_cover_art(const Model::Song&, Model::Cover& cov, const std::string&);

    void update_metadata(Model::Song updated_song, const Model::Song old_song);
private:
};

#endif
