#include "manager/UserManager.h"

#include "database/UserRepository.h"
#include "type/UserFilter.h"
#include "type/SaltFilter.h"
#include "utility/MetadataRetriever.h"
#include "utility/PasswordEncryption.h"

namespace manager {
    UserManager::UserManager(const icarus_lib::binary_path & bConf) : m_bConf(bConf) { }


    icarus_lib::register_result UserManager::registerUser(icarus_lib::user& user) {
		icarus_lib::register_result result;
		result.username = user.username;
		printUser(user);

		utility::PasswordEncryption passEnc;
		auto hashed = passEnc.hashPassword(user);
		user.password = hashed.hash_password;

		database::UserRepository usrRepo(m_bConf);
		usrRepo.saveUserRecord(user);

		icarus_lib::user usr;
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


    bool UserManager::doesUserExist(const icarus_lib::user& user) {
		database::UserRepository userRepo(m_bConf);

		return userRepo.doesUserRecordExist(user, type::UserFilter::username);
    }

    bool UserManager::validatePassword(const icarus_lib::user& user) {
		database::UserRepository userRepo(m_bConf);
		icarus_lib::user usr;
		usr.username = user.username;
		usr = userRepo.retrieveUserRecord(usr, type::UserFilter::username);

		icarus_lib::pass_sec userSec;
		userSec.user_id = usr.id;
		userSec = userRepo.retrieverUserSaltRecord(userSec, type::SaltFilter::userId);
		userSec.hash_password = usr.password;

		utility::PasswordEncryption passEnc;

		return passEnc.isPasswordValid(user, userSec);
    }


    void UserManager::printUser(const icarus_lib::user& user) {
		std::cout << "\nuser info\n";
		std::cout << "id: " << user.id << "\n";
		std::cout << "firstname: " << user.firstname << "\n";
		std::cout << "lastname: " << user.lastname << "\n";
		std::cout << "phone: " << user.phone << "\n";
		std::cout << "email: " << user.email << "\n";
		std::cout << "username: " << user.username << "\n";
		std::cout << "password: " << user.password<< "\n";
    }

}
