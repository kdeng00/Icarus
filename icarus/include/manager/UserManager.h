#ifndef USERMANAGER_H_
#define USERMANAGER_H_

#include <iostream>

#include "icarus_lib/icarus.h"

#include "database/UserRepository.h"
#include "dto/LoginResultDto.hpp"
#include "manager/BaseManager.hpp"
#include "type/SaltFilter.h"
#include "type/UserFilter.h"
#include "utility/MetadataRetriever.h"
#include "utility/PasswordEncryption.h"

namespace manager {
    template<typename user_val = icarus_lib::user>
    class UserManager : BaseManager<icarus_lib::binary_path>
    {
    public:
        UserManager(const icarus_lib::binary_path &b_conf) :
                        BaseManager(b_conf)
        {
        }


        template<typename register_result_val = icarus_lib::register_result>
        register_result_val registerUser(user_val &user)
        {
		    register_result_val result;
		    result.username = user.username;
		    printUser(user);

    		utility::PasswordEncryption passEnc;
    		auto hashed = passEnc.hashPassword(user);
    		user.password = hashed.hash_password;

    		database::UserRepository<usr_val> usrRepo(m_bConf);
    		usrRepo.saveUserRecord(user);

    		user_val usr;
    		usr.username = user.username;

    		std::cout << usr.username << "\n";
    		usr = usrRepo.retrieveUserRecord(usr, type::UserFilter::username);
    		hashed.hash_password = usr.password;
    		hashed.user_id = usr.id;

    		usrRepo.saveUserSalt(hashed);
    		result.registered = true;
    		result.message = "successfully registered";
    		printUser(usr);

    		return result;
        }


        bool doesUserExist(const user_val &user)
        {
		    database::UserRepository<user_val> userRepo(m_bConf);

    		return userRepo.doesUserRecordExist(user, type::UserFilter::username);
        }

        bool validatePassword(const user_val &user)
        {
		    database::UserRepository<user_val> userRepo(m_bConf);
		    user_val usr;
		    usr.username = user.username;
		    usr = userRepo.retrieveUserRecord(usr, type::UserFilter::username);

    		icarus_lib::pass_sec userSec;
    		userSec.user_id = usr.id;
    		userSec = userRepo.retrieverUserSaltRecord(userSec, type::SaltFilter::userId);
    		userSec.hash_password = usr.password;

    		utility::PasswordEncryption passEnc;

    		return passEnc.isPasswordValid(user, userSec);
        }

        void printUser(const user_val &user)
        {
		    std::cout << "\nuser info\n";
		    std::cout << "id: " << user.id << "\n";
		    std::cout << "firstname: " << user.firstname << "\n";
		    std::cout << "lastname: " << user.lastname << "\n";
		    std::cout << "phone: " << user.phone << "\n";
		    std::cout << "email: " << user.email << "\n";
		    std::cout << "username: " << user.username << "\n";
		    std::cout << "password: " << user.password<< "\n";
        }
    private:
    };
}

#endif
