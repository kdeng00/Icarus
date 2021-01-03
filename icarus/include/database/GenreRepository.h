#ifndef GENREREPOSITORY_H_
#define GENREREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include "icarus_lib/icarus.h"

#include "database/BaseRepository.hpp"
#include "type/GenreFilter.h"

namespace database {
    class GenreRepository : public BaseRepository {
    public:
        GenreRepository(const icarus_lib::binary_path&);

        std::vector<icarus_lib::genre> retrieveRecords();

        std::pair<icarus_lib::genre, int> retrieveRecordWithSongCount(icarus_lib::genre&, type::GenreFilter);

        icarus_lib::genre retrieveRecord(icarus_lib::genre&, type::GenreFilter);

        bool doesGenreExist(const icarus_lib::genre&, type::GenreFilter);

        void saveRecord(const icarus_lib::genre&);
        void deleteRecord(const icarus_lib::genre&, type::GenreFilter);
    private:
        std::vector<icarus_lib::genre> parseRecords(MYSQL_STMT*);

        std::pair<icarus_lib::genre, int> parseRecordWithSongCount(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(icarus_lib::genre&,
                std::tuple<char*>&);
        std::shared_ptr<MYSQL_BIND> valueBindWithSongCount(icarus_lib::genre&,
                std::tuple<char*>&, int&);

        std::tuple<char*> metadataBuffer();

        icarus_lib::genre parseRecord(MYSQL_STMT*);
    };
}

#endif
