#ifndef PASSWORDENCRYPTION_H_
#define PASSWORDENCRYPTION_H_

#include <string>

#include "icarus_lib/icarus.h"

namespace utility {
class PasswordEncryption {
public:
    icarus_lib::pass_sec hashPassword(const icarus_lib::user&);

    bool isPasswordValid(const icarus_lib::user&, const icarus_lib::pass_sec&);
private:
    constexpr int saltSize() noexcept;
};
}

#endif
