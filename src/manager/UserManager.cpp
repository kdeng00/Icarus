#include "manager/UserManager.h"

#include "database/UserRepository.h"
#include "type/UserFilter.h"
#include "type/SaltFilter.h"
#include "utility/MetadataRetriever.h"
#include "utility/PasswordEncryption.h"

namespace manager {
UserManager::UserManager(const model::BinaryPath& bConf) : m_bConf(bConf) { }

bool UserManager::doesUserExist(const model::User& user)
{
    database::UserRepository userRepo(m_bConf);

    return userRepo.doesUserRecordExist(user, type::UserFilter::username);
}

bool UserManager::validatePassword(const model::User& user)
{
    database::UserRepository userRepo(m_bConf);
    model::User usr;
    usr.username = user.username;
    usr = userRepo.retrieveUserRecord(usr, type::UserFilter::username);

    model::PassSec userSec;
    userSec.userId = usr.id;
    userSec = userRepo.retrieverUserSaltRecord(userSec, type::SaltFilter::userId);
    userSec.hashPassword = usr.password;

    utility::PasswordEncryption passEnc;

    return passEnc.isPasswordValid(user, userSec);
}


void UserManager::printUser(const model::User& user)
{
    std::cout << "\nuser info" << std::endl;
    std::cout << "id: " << user.id << std::endl;
    std::cout << "firstname: " << user.firstname << std::endl;
    std::cout << "lastname: " << user.lastname << std::endl;
    std::cout << "phone: " << user.phone << std::endl;
    std::cout << "email: " << user.email << std::endl;
    std::cout << "username: " << user.username << std::endl;
    std::cout << "password: " << user.password<< std::endl;
}

void UserManager::registerUser(model::User& user)
{
    printUser(user);

    utility::PasswordEncryption passEnc;
    auto hashed = passEnc.hashPassword(user);
    user.password = hashed.hashPassword;

    database::UserRepository usrRepo(m_bConf);
    usrRepo.saveUserRecord(user);

    model::User usr;
    usr.username = user.username;

    std::cout << usr.username << std::endl;
    usr = usrRepo.retrieveUserRecord(usr, type::UserFilter::username);
    hashed.hashPassword = usr.password;
    hashed.userId = usr.id;

    usrRepo.saveUserSalt(hashed);
    printUser(usr);
}


model::User UserManager::userDtoConv(dto::UserDto::ObjectWrapper& userDto)
{
    model::User user;

    user.id = (userDto->userId.getPtr() == nullptr) ? 0 : userDto->userId->getValue();
    user.firstname = (userDto->firstname == nullptr) ? "" : userDto->firstname->c_str();
    user.lastname = (userDto->lastname == nullptr) ? "" : userDto->lastname->c_str();
    user.phone = (userDto->phone == nullptr) ? "" : userDto->phone->c_str();
    user.email = (userDto->email == nullptr) ? "" : userDto->email->c_str();
    user.username = (userDto->username == nullptr) ? "" : userDto->username->c_str();
    user.password = (userDto->password == nullptr) ? "" : userDto->password->c_str();

    return user;
}
}
