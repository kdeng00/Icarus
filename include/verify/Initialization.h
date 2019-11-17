#ifndef INITIALIZATION_H_
#define INITIALIZATION_H_

#include <string>

#include "model/Models.h"

namespace verify {
    class Initialization {
    public:
        static bool skipVerification(const std::string&);

        static void checkIcarus(const model::BinaryPath&);
    private:
        static bool confirmConfigAuth(const model::BinaryPath&);
        static bool confirmConfigDatabase(const model::BinaryPath&);
        static bool confirmConfigPaths(const model::BinaryPath&);

        static void failedConfirmation();
    };
}

#endif
