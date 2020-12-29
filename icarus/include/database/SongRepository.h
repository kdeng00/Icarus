#ifndef SONGREPOSITORY_H_
#define SONGREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include "icarus_lib/icarus.h"
#include <mysql/mysql.h>

#include "database/BaseRepository.h"
#include "type/SongFilter.h"

namespace database {
    class SongRepository : public BaseRepository {
    public:
        SongRepository(const icarus_lib::binary_path &);

        std::vector<icarus_lib::song> retrieveRecords();

        icarus_lib::song retrieveRecord(const icarus_lib::song&, type::SongFilter = type::SongFilter::id);

        bool doesSongExist(const icarus_lib::song&, type::SongFilter = type::SongFilter::id);
        bool deleteRecord(const icarus_lib::song&);

        void saveRecord(const icarus_lib::song&);
        void updateRecord(const icarus_lib::song&);
    private:
        std::shared_ptr<MYSQL_BIND> valueBind(icarus_lib::song&, 
            std::tuple<char*, char*, char*, char*, char*, char*>&);

        std::tuple<char*, char*, char*, char*, char*, char*> metadataBuffer();

        std::vector<icarus_lib::song> parseRecords(MYSQL_STMT*);

        icarus_lib::song parseRecord(MYSQL_STMT*);
    };
}

#endif
