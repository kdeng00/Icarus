#ifndef GENREREPOSITORY_H_
#define GENREREPOSITORY_H_

#include <utility>
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

        std::pair<model::Genre, int> retrieveRecordWithSongCount(model::Genre&, type::GenreFilter);

        model::Genre retrieveRecord(model::Genre&, type::GenreFilter);

        bool doesGenreExist(const model::Genre&, type::GenreFilter);

        void saveRecord(const model::Genre&);
        void deleteRecord(const model::Genre&, type::GenreFilter);
    private:
        std::vector<model::Genre> parseRecords(MYSQL_STMT*);

        std::pair<model::Genre, int> parseRecordWithSongCount(MYSQL_STMT*);

        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        model::Genre parseRecord(MYSQL_RES*);
        model::Genre parseRecord(MYSQL_STMT*);
    };
}

#endif
