#ifndef PASSWORDENCRYPTION_H_
#define PASSWORDENCRYPTION_H_

#include <string>

#include "model/Models.h"

namespace utility {
    class PasswordEncryption {
    public:
        model::PassSec hashPassword(const model::User&);
    private:
        int saltSize();
    };
}

#endif
