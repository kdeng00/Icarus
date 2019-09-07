#ifndef GENREREPOSITORY_H_
#define GENREREPOSITORY_H_

#include <vector>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/GenreFilter.h"

namespace database
{
    class GenreRepository : public BaseRepository
    {
    public:
        GenreRepository(const model::BinaryPath&);

        std::vector<model::Genre> retrieveRecords();

        model::Genre retrieveRecord(model::Genre&, type::GenreFilter);

        bool doesGenreExist(const model::Genre&, type::GenreFilter);

        void saveRecord(const model::Genre&);
    private:
        std::vector<model::Genre> parseRecords(MYSQL_STMT*);

        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        model::Genre parseRecord(MYSQL_RES*);
        model::Genre parseRecord(MYSQL_STMT*);
    };
}

#endif
