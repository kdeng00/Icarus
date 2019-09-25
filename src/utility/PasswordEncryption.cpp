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


bool PasswordEncryption::isPasswordValid(const model::User& user, const model::PassSec& userSalt)
{
    std::unique_ptr<char[]> passwordHashed(new char[BCRYPT_HASHSIZE]);

    bcrypt_hashpw(user.password.c_str(), userSalt.salt.c_str(), passwordHashed.get());

    return (userSalt.hashPassword.compare(passwordHashed.get()) == 0) ? true : false;
}


int PasswordEncryption::saltSize()
{
    constexpr auto size = 10000;

    return size;
}
}
