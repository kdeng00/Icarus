#ifndef ALBUMREPOSITORY_H_
#define ALBUMREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include "icarus_lib/icarus.h"

#include "database/BaseRepository.h"
#include "type/AlbumFilter.h"

namespace database {
    class AlbumRepository : public BaseRepository {
    public:
        AlbumRepository(const icarus_lib::binary_path&);

        std::vector<icarus_lib::album> retrieveRecords();

        std::pair<icarus_lib::album, int> retrieveRecordWithSongCount(icarus_lib::album&, type::AlbumFilter);

        icarus_lib::album retrieveRecord(icarus_lib::album&, type::AlbumFilter);

        bool doesAlbumExists(const icarus_lib::album&, type::AlbumFilter);

        void saveAlbum(const icarus_lib::album&);
        void deleteAlbum(const icarus_lib::album&, type::AlbumFilter);
    private:
        std::vector<icarus_lib::album> parseRecords(MYSQL_STMT*);

        std::pair<icarus_lib::album, int> parseRecordWithSongCount(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(icarus_lib::album&, 
                std::tuple<char*, char*>&);
        std::shared_ptr<MYSQL_BIND> valueBindWithSongCount(icarus_lib::album&, 
                std::tuple<char*, char*>&, int&);

        std::tuple<char*, char*> metadataBuffer();

        icarus_lib::album parseRecord(MYSQL_STMT*);
    };
}

#endif
