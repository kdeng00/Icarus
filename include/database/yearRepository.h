#ifndef YEARREPOSITORY_H_
#define YEARREPOSITORY_H_

#include "database/base_repository.h"
#include "models/models.h"

namespace Database
{
    class yearRepository : public base_repository
    {
    public:
        yearRepository(const Model::BinaryPath&);
    private:
    };
}

#endif
