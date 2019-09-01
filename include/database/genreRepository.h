#ifndef GENREREPOSITORY_H_
#define GENREREPOSITORY_H_

#include "database/base_repository.h"
#include "models/models.h"

namespace Database
{
    class genreRepository : public base_repository
    {
    public:
        genreRepository(const Model::BinaryPath&);
    private:
    };
}

#endif
