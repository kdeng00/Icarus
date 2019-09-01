#ifndef SONGREPOSITORY_H_
#define SONGREPOSITORY_H_

#include <memory>
#include <vector>

#include <mysql/mysql.h>

#include "database/base_repository.h"
#include "models/models.h"
#include "types/songFilter.h"

namespace Database
{
    class songRepository : public base_repository
    {
    public:
        songRepository(const std::string&);
        songRepository(const Model::BinaryPath&);

        std::vector<Model::Song> retrieveRecords();

        Model::Song retrieveRecord(Model::Song&, songFilter);

        void deleteRecord(const Model::Song&);
        void saveRecord(const Model::Song&);
    private:
        std::vector<Model::Song> parseRecords(MYSQL_RES*);

        Model::Song parseRecord(MYSQL_RES*);
    };
}

#endif
