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

        Model::Cover retrieveRecord(Model::Cover&, Type::coverFilter);
        
        bool doesCoverArtExist(const Model::Cover&, Type::coverFilter);

        void deleteRecord(const Model::Cover&);
        void saveRecord(const Model::Cover&);
    private:
        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        Model::Cover parseRecord(MYSQL_RES*);
        Model::Cover parseRecord(MYSQL_STMT*);
    };
}

#endif
