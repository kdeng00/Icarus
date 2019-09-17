#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#include "model/Models.h"

namespace verify
{
    class Initialization
    {
    public:
        static void checkIcarus(const model::BinaryPath&);
    private:
        bool confirmConfigAuth(const model::BinaryPath&);
        bool confirmConfigDatabase(const model::BinaryPath&);
        bool confirmConfigPaths(const model::BinaryPath&);
    };
}

#endif
