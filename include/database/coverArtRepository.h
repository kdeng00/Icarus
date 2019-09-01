#ifndef COVERARTREPOSITORY_H_
#define COVERARTREPOSITORY_H_

#include <mysql/mysql.h>

#include "database/base_repository.h"
#include "models/models.h"
#include "types/coverFilter.h"

namespace Database
{
    class coverArtRepository : public base_repository
    {
    public:
        coverArtRepository(const std::string&);
        coverArtRepository(const Model::BinaryPath&);

        Model::Cover retrieveRecord(Model::Cover&, coverFilter);

        void deleteRecord(const Model::Cover&);
        void saveRecord(const Model::Cover&);
    private:
        Model::Cover parseRecord(MYSQL_RES*);
    };
}

#endif
