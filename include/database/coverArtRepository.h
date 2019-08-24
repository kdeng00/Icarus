#ifndef COVERARTREPOSITORY_H_
#define COVERARTREPOSITORY_H_

#include <mysql/mysql.h>

#include "database/base_repository.h"
#include "models/models.h"

class coverArtRepository : public base_repository
{
public:
    coverArtRepository(const std::string&);

    void saveRecord(const Cover&);
private:
};

#endif
