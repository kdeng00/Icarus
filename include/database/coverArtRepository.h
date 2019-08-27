#ifndef COVERARTREPOSITORY_H_
#define COVERARTREPOSITORY_H_

#include <mysql/mysql.h>

#include "database/base_repository.h"
#include "models/models.h"
#include "types/coverFilter.h"

class coverArtRepository : public base_repository
{
public:
    coverArtRepository(const std::string&);

    Cover retrieveRecord(Cover&, coverFilter);

    void deleteRecord(const Cover&);
    void saveRecord(const Cover&);
private:
    Cover parseRecord(MYSQL_RES*);
};

#endif
