#ifndef GENREREPOSITORY_H_
#define GENREREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include "icarus_lib/icarus.h"

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/GenreFilter.h"

namespace database {
    class GenreRepository : public BaseRepository {
    public:
        GenreRepository(const icarus_lib::binary_path&);

        std::vector<model::Genre> retrieveRecords();

        std::pair<model::Genre, int> retrieveRecordWithSongCount(model::Genre&, type::GenreFilter);

        model::Genre retrieveRecord(model::Genre&, type::GenreFilter);

        bool doesGenreExist(const model::Genre&, type::GenreFilter);

        void saveRecord(const model::Genre&);
        void deleteRecord(const model::Genre&, type::GenreFilter);
    private:
        std::vector<model::Genre> parseRecords(MYSQL_STMT*);

        std::pair<model::Genre, int> parseRecordWithSongCount(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(model::Genre&,
                std::tuple<char*>&);
        std::shared_ptr<MYSQL_BIND> valueBindWithSongCount(model::Genre&,
                std::tuple<char*>&, int&);

        std::tuple<char*> metadataBuffer();

        model::Genre parseRecord(MYSQL_STMT*);
    };
}

#endif
