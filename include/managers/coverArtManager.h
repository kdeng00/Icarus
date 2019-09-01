#ifndef COVERARTMANAGER_H_
#define COVERARTMANAGER_H_

#include <string>

#include "models/models.h"

class coverArtManager
{
public:
    coverArtManager(const std::string&);

    Model::Cover saveCover(const Model::Song&, std::string&, const std::string&);
private:
    std::string path;
};

#endif
