#ifndef ALBUMREPOSITORY_H_
#define ALBUMREPOSITORY_H_

#include "database/base_repository.h"
#include "models/models.h"

namespace Database
{
    class albumRepository : public base_repository
    {
    public:
        albumRepository(const Model::BinaryPath&);
    private:
    };
}

#endif
