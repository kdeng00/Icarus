#ifndef GENREREPOSITORY_H_
#define GENREREPOSITORY_H_

#include "database/base_repository.h"
#include "models/models.h"
#include "types/genreFilter.h"

namespace Database
{
    class genreRepository : public base_repository
    {
    public:
        genreRepository(const Model::BinaryPath&);

        Model::Genre retrieveRecord(Model::Genre&, Type::genreFilter);

        void saveRecord(const Model::Genre&);
    private:
        Model::Genre parseRecord(MYSQL_RES*);
    };
}

#endif
