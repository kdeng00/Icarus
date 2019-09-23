#include "utility/PasswordEncryption.h"

#include <iostream>
#include <memory>

#include <bcrypt.h>

namespace utility {

model::PassSec PasswordEncryption::hashPassword(const model::User& user)
{
    model::PassSec passSec;

    std::unique_ptr<char[]> salt(new char[BCRYPT_HASHSIZE]);
    std::unique_ptr<char[]> hash(new char[BCRYPT_HASHSIZE]);

    std::cout << "generating salt" << std::endl;
    bcrypt_gensalt(saltSize(), salt.get());
    std::cout << "hashing password" << std::endl;
    bcrypt_hashpw(user.password.c_str(), salt.get(), hash.get());

    passSec.salt = salt.get();
    passSec.hashPassword = hash.get();

    std::cout << "hash: " << passSec.hashPassword << std::endl;
    std::cout << "salt: " << passSec.salt << std::endl;

    return passSec;
}

int PasswordEncryption::saltSize()
{
    constexpr auto size = 10000;

    return size;
}
}
