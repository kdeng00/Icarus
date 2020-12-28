#ifndef COVERARTREPOSITORY_H_
#define COVERARTREPOSITORY_H_

#include <memory>
#include <vector>

#include <mysql/mysql.h>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/CoverFilter.h"

namespace database {
    class CoverArtRepository : public BaseRepository {
    public:
        CoverArtRepository(const model::BinaryPath&);

        std::vector<model::Cover> retrieveRecords();

        model::Cover retrieveRecord(model::Cover&, type::CoverFilter);
        
        bool doesCoverArtExist(const model::Cover&, type::CoverFilter);

        void deleteRecord(const model::Cover&, type::CoverFilter = type::CoverFilter::id);
        void saveRecord(const model::Cover&);
        void updateRecord(const model::Cover&);
    private:
        std::vector<model::Cover> parseRecords(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(model::Cover&,
                std::tuple<char*, char*>&);

        std::tuple<char*, char*> metadataBuffer();

        model::Cover parseRecord(MYSQL_STMT*);
    };
}

#endif
