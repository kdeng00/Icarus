#ifndef YEARREPOSITORY_H_
#define YEARREPOSITORY_H_

#include "database/base_repository.h"
#include "models/models.h"
#include "types/yearFilter.h"

namespace Database
{
    class yearRepository : public base_repository
    {
    public:
        yearRepository(const Model::BinaryPath&);

        Model::Year retrieveRecord(Model::Year&, Type::yearFilter);

        void saveRecord(const Model::Year&);
    private:
        Model::Year parseRecord(MYSQL_RES*);
    };
}

#endif
