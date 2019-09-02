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

        void saveRecord(const Model::Artist&);
    private:
        Model::Artist parseRecord(MYSQL_RES*);
    };
}

#endif
