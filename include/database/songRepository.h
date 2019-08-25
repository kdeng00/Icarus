#ifndef SONGREPOSITORY_H_
#define SONGREPOSITORY_H_

#include <memory>
#include <vector>

#include <mysql/mysql.h>

#include "database/base_repository.h"
#include "models/models.h"
#include "types/songFilter.h"

class songRepository : public base_repository
{
public:
    songRepository(const std::string&);

    Song retrieveRecord(Song&, songFilter);

    void saveRecord(const Song&);
private:
    std::vector<Song> parseRecords(MYSQL_RES*);

    std::unique_ptr<MYSQL_BIND*> bindParams(const Song&);
    //std::unique_ptr<MYSQL_BIND[]> bindParams(const Song&);
    //std::shared_ptr<MYSQL_BIND> bindParams(const Song&);

    Song parseRecord(MYSQL_RES*);
};

#endif
