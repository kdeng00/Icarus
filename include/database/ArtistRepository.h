#ifndef ARTISTREPOSITORY_H_
#define ARTISTREPOSITORY_H_

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/ArtistFilter.h"

namespace database
{
    class ArtistRepository : public BaseRepository
    {
    public:
        ArtistRepository(const model::BinaryPath&);

        model::Artist retrieveRecord(model::Artist&, type::ArtistFilter);

        bool doesArtistExist(const model::Artist&, type::ArtistFilter);

        void saveRecord(const model::Artist&);
    private:
        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        model::Artist parseRecord(MYSQL_RES*);
        model::Artist parseRecord(MYSQL_STMT*);
    };
}

#endif
