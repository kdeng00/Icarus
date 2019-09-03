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

        bool doesGenreExist(const Model::Genre&, Type::genreFilter);

        void saveRecord(const Model::Genre&);
    private:
        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        Model::Genre parseRecord(MYSQL_RES*);
        Model::Genre parseRecord(MYSQL_STMT*);
    };
}

#endif
