#ifndef SONGREPOSITORY_H_
#define SONGREPOSITORY_H_

#include <memory>
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
        SongRepository(const std::string&);
        SongRepository(const model::BinaryPath&);

        std::vector<model::Song> retrieveRecords();

        model::Song retrieveRecord(model::Song&, type::SongFilter);

        bool doesSongExist(const model::Song&, type::SongFilter);
        bool deleteRecord(const model::Song&);

        void saveRecord(const model::Song&);
        void updateRecord(const model::Song&);
    private:
        std::vector<model::Song> parseRecords(MYSQL_RES*); // TODO: to be removed
        std::vector<model::Song> parseRecords(MYSQL_STMT*);

        model::Song parseRecord(MYSQL_RES*); // TODO: to be removed
        model::Song parseRecord(MYSQL_STMT*);
    };
}

#endif
