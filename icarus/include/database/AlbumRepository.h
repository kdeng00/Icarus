#ifndef ALBUMREPOSITORY_H_
#define ALBUMREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include "icarus_lib/icarus.h"

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/AlbumFilter.h"

namespace database {
    class AlbumRepository : public BaseRepository {
    public:
        AlbumRepository(const icarus_lib::binary_path&);

        std::vector<model::Album> retrieveRecords();

        std::pair<model::Album, int> retrieveRecordWithSongCount(model::Album&, type::AlbumFilter);

        model::Album retrieveRecord(model::Album&, type::AlbumFilter);

        bool doesAlbumExists(const model::Album&, type::AlbumFilter);

        void saveAlbum(const model::Album&);
        void deleteAlbum(const model::Album&, type::AlbumFilter);
    private:
        std::vector<model::Album> parseRecords(MYSQL_STMT*);

        std::pair<model::Album, int> parseRecordWithSongCount(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(model::Album&, 
                std::tuple<char*, char*>&);
        std::shared_ptr<MYSQL_BIND> valueBindWithSongCount(model::Album&, 
                std::tuple<char*, char*>&, int&);

        std::tuple<char*, char*> metadataBuffer();

        model::Album parseRecord(MYSQL_STMT*);
    };
}

#endif
