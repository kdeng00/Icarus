#ifndef ARTISTREPOSITORY_H_
#define ARTISTREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include "icarus_lib/icarus.h"

#include "database/BaseRepository.hpp"
#include "type/ArtistFilter.h"

namespace database {
    class ArtistRepository : public BaseRepository {
    public:
        ArtistRepository(const icarus_lib::binary_path&);

        std::vector<icarus_lib::artist> retrieveRecords();

        std::pair<icarus_lib::artist, int> retrieveRecordWithSongCount(
                icarus_lib::artist&, type::ArtistFilter);

        icarus_lib::artist retrieveRecord(icarus_lib::artist&, type::ArtistFilter);

        bool doesArtistExist(const icarus_lib::artist&, type::ArtistFilter);

        void saveRecord(const icarus_lib::artist&);
        void deleteArtist(const icarus_lib::artist&, type::ArtistFilter);
    private:
        std::vector<icarus_lib::artist> parseRecords(MYSQL_STMT*);

        std::pair<icarus_lib::artist, int> parseRecordWithSongCount(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(icarus_lib::artist&,
                std::tuple<char*>&);
        std::shared_ptr<MYSQL_BIND> valueBindWithSongCount(icarus_lib::artist&,
                std::tuple<char*>&, int&);

        std::tuple<char*> metadataBuffer();

        icarus_lib::artist parseRecord(MYSQL_STMT*);
    };
}

#endif
