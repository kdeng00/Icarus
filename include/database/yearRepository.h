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

        bool doesYearExist(const Model::Year&, Type::yearFilter);

        void saveRecord(const Model::Year&);
    private:
        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        Model::Year parseRecord(MYSQL_RES*);
        Model::Year parseRecord(MYSQL_STMT*);
    };
}

#endif
