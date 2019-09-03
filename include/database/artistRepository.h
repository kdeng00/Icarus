#ifndef ARTISTREPOSITORY_H_
#define ARTISTREPOSITORY_H_

#include "database/base_repository.h"
#include "models/models.h"
#include "types/artistFilter.h"

namespace Database
{
    class artistRepository : public base_repository
    {
    public:
        artistRepository(const Model::BinaryPath&);

        Model::Artist retrieveRecord(Model::Artist&, Type::artistFilter);

        bool doesArtistExist(const Model::Artist&, Type::artistFilter);

        void saveRecord(const Model::Artist&);
    private:
        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        Model::Artist parseRecord(MYSQL_RES*);
        Model::Artist parseRecord(MYSQL_STMT*);
    };
}

#endif
