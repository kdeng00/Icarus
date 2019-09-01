#ifndef ARTISTREPOSITORY_H_
#define ARTISTREPOSITORY_H_

#include "database/base_repository.h"
#include "models/models.h"
#include "types/coverFilter.h"

namespace Database
{
    class artistRepository : public base_repository
    {
    public:
        artistRepository(const Model::BinaryPath&);
    private:
    };
}

#endif
