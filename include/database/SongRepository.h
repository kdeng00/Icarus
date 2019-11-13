#ifndef SONGREPOSITORY_H_
#define SONGREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include <mysql/mysql.h>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/SongFilter.h"

namespace database
{
    class SongRepository : public BaseRepository
    {
    public:
        SongRepository(const model::BinaryPath&);

        std::vector<model::Song> retrieveRecords();

        model::Song retrieveRecord(const model::Song&, type::SongFilter);

        bool doesSongExist(const model::Song&, type::SongFilter);
        bool deleteRecord(const model::Song&);

        void saveRecord(const model::Song&);
        void updateRecord(const model::Song&);
    private:
        std::shared_ptr<MYSQL_BIND> valueBind(model::Song&, 
            std::tuple<char*, char*, char*, char*, char*, char*>&);

        std::tuple<char*, char*, char*, char*, char*, char*> metadataBuffer();

        std::vector<model::Song> parseRecords(MYSQL_STMT*);

        model::Song parseRecord(MYSQL_STMT*);
    };
}

#endif
