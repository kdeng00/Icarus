#include "manager/UserManager.h"

#include "database/UserRepository.h"
#include "dto/LoginResultDto.hpp"
#include "utility/PasswordEncryption.h"


using icarus_lib::binary_path;
using icarus_lib::pass_sec;
using icarus_lib::register_result;
using icarus_lib::user;

using utility::PasswordEncryption;


namespace manager
{

    UserManager::UserManager(const binary_path &b_conf) :
                    BaseManager(b_conf)
    {
    }


    register_result UserManager::registerUser(user &usr)
    {
        register_result result;
        result.username = usr.username;
        printUser(usr);

        PasswordEncryption passEnc;
        auto hashed = passEnc.hashPassword(usr);
        usr.password = hashed.hash_password;

        database::UserRepository usrRepo(m_config);
        usrRepo.saveUserRecord(usr);

        user new_usr;
        new_usr.username = usr.username;

        std::cout << new_usr.username << "\n";
        new_usr = usrRepo.retrieveUserRecord(new_usr, type::UserFilter::username);
        hashed.hash_password = new_usr.password;
        hashed.user_id = new_usr.id;

        usrRepo.saveUserSalt(hashed);
        result.registered = true;
        result.message = "successfully registered";
        printUser(new_usr);

        return result;
    }


    bool UserManager::doesUserExist(const user &usr)
    {
        database::UserRepository userRepo(m_config);

        return userRepo.user_exists(usr);
    }

    bool UserManager::validatePassword(const user &usr)
    {
        database::UserRepository userRepo(m_config);
        user some_usr;
        some_usr.username = usr.username;
        some_usr = userRepo.retrieveUserRecord(some_usr, type::UserFilter::username);

        pass_sec userSec;
        userSec.user_id = some_usr.id;
        userSec = userRepo.retrieverUserSaltRecord(userSec, type::SaltFilter::userId);
        userSec.hash_password = some_usr.password;

        PasswordEncryption passEnc;

        return passEnc.isPasswordValid(usr, userSec);
    }

    void UserManager::printUser(const user &usr)
    {
        std::cout << "\nuser info\n";
        std::cout << "id: " << usr.id << "\n";
        std::cout << "firstname: " << usr.firstname << "\n";
        std::cout << "lastname: " << usr.lastname << "\n";
        std::cout << "phone: " << usr.phone << "\n";
        std::cout << "email: " << usr.email << "\n";
        std::cout << "username: " << usr.username << "\n";
        std::cout << "password: " << usr.password<< "\n";
    }
}
