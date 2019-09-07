#ifndef COVERARTREPOSITORY_H_
#define COVERARTREPOSITORY_H_

#include <vector>

#include <mysql/mysql.h>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/CoverFilter.h"

namespace database
{
    class CoverArtRepository : public BaseRepository
    {
    public:
        CoverArtRepository(const std::string&);
        CoverArtRepository(const model::BinaryPath&);

        std::vector<model::Cover> retrieveRecords();

        model::Cover retrieveRecord(model::Cover&, type::CoverFilter);
        
        bool doesCoverArtExist(const model::Cover&, type::CoverFilter);

        void deleteRecord(const model::Cover&);
        void saveRecord(const model::Cover&);
    private:
        std::vector<model::Cover> parseRecords(MYSQL_STMT*);

        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        model::Cover parseRecord(MYSQL_RES*);
        model::Cover parseRecord(MYSQL_STMT*);
    };
}

#endif
