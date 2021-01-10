#ifndef COVERARTREPOSITORY_H_
#define COVERARTREPOSITORY_H_

#include <memory>
#include <vector>

#include "icarus_lib/icarus.h"
#include <mysql/mysql.h>

#include "database/BaseRepository.h"
#include "type/CoverFilter.h"

namespace database {
    class CoverArtRepository : public BaseRepository {
    public:
        CoverArtRepository(const icarus_lib::binary_path &);

        std::vector<icarus_lib::cover> retrieveRecords();

        icarus_lib::cover retrieveRecord(icarus_lib::cover&, type::CoverFilter);
        
        bool doesCoverArtExist(const icarus_lib::cover&, type::CoverFilter);

        void deleteRecord(const icarus_lib::cover&, type::CoverFilter = type::CoverFilter::id);
        void saveRecord(const icarus_lib::cover&);
        void updateRecord(const icarus_lib::cover&);
    private:
        std::vector<icarus_lib::cover> parseRecords(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(icarus_lib::cover&,
                std::tuple<char*, char*>&);

        std::tuple<char*, char*> metadataBuffer();

        icarus_lib::cover parseRecord(MYSQL_STMT*);
    };
}

#endif
