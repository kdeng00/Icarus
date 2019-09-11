#ifndef ARTISTREPOSITORY_H_
#define ARTISTREPOSITORY_H_

#include <utility>
#include <vector>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/ArtistFilter.h"

namespace database
{
    class ArtistRepository : public BaseRepository
    {
    public:
        ArtistRepository(const model::BinaryPath&);

        std::vector<model::Artist> retrieveRecords();

        std::pair<model::Artist, int> retrieveRecordWithSongCount(model::Artist&, type::ArtistFilter);

        model::Artist retrieveRecord(model::Artist&, type::ArtistFilter);

        bool doesArtistExist(const model::Artist&, type::ArtistFilter);

        void saveRecord(const model::Artist&);
        void deleteArtist(const model::Artist&, type::ArtistFilter);
    private:
        std::vector<model::Artist> parseRecords(MYSQL_STMT*);

        std::pair<model::Artist, int> parseRecordWithSongCount(MYSQL_STMT*);

        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        model::Artist parseRecord(MYSQL_RES*);
        model::Artist parseRecord(MYSQL_STMT*);
    };
}

#endif
