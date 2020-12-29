#include "utility/PasswordEncryption.h"

#include <iostream>
#include <memory>

#include <bcrypt.h>

namespace utility {

    icarus_lib::pass_sec PasswordEncryption::hashPassword(const icarus_lib::user& user) {
        icarus_lib::pass_sec passSec;

        std::unique_ptr<char[]> salt(new char[BCRYPT_HASHSIZE]);
        std::unique_ptr<char[]> hash(new char[BCRYPT_HASHSIZE]);

        std::cout << "generating salt\n";
        bcrypt_gensalt(saltSize(), salt.get());
        std::cout << "hashing password\n";
        bcrypt_hashpw(user.password.c_str(), salt.get(), hash.get());

        passSec.salt = salt.get();
        passSec.hash_password = hash.get();

        std::cout << "hash: " << passSec.hash_password << "\n";
        std::cout << "salt: " << passSec.salt << "\n";

        return passSec;
    }


    bool PasswordEncryption::isPasswordValid(const icarus_lib::user& user, 
            const icarus_lib::pass_sec& userSalt) {
        std::unique_ptr<char[]> passwordHashed(new char[BCRYPT_HASHSIZE]);

        bcrypt_hashpw(user.password.c_str(), userSalt.salt.c_str(), passwordHashed.get());

        return (userSalt.hash_password.compare(passwordHashed.get()) == 0) ? true : false;
    }


    constexpr int PasswordEncryption::saltSize() noexcept { return 10000; }
}
