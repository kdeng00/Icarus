#include "database/UserRepository.h"

#include <string>
#include <sstream>
#include <tuple>
#include <cstring>

namespace database {

    UserRepository::UserRepository(const icarus_lib::binary_path & bConf) : BaseRepository(bConf) { }


    icarus_lib::user UserRepository::retrieveUserRecord(icarus_lib::user& user, 
            type::UserFilter filter = type::UserFilter::username) {
        std::stringstream qry;
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        qry << "SELECT * FROM User WHERE ";

        MYSQL_BIND params[1];
        std::memset(params, 0, sizeof(params));

        auto userLength = user.username.size();
        switch (filter) {
            case type::UserFilter::id:
                qry << "UserId = ?";

                params[0].buffer_type = MYSQL_TYPE_LONG;
                params[0].buffer = (char*)&user.id;
                params[0].length = nullptr;
                params[0].is_null = 0;

                break;
            case type::UserFilter::username:
                qry << "Username = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char*)user.username.c_str();
                params[0].length = &userLength;
                params[0].is_null = 0;
               break;
          default:
                break;
        }

        qry << " LIMIT 1";

        const auto query = qry.str();
        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, params);
        status = mysql_stmt_execute(stmt);

        user = parseRecord(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);

        return user;
    }

    icarus_lib::pass_sec UserRepository::retrieverUserSaltRecord(icarus_lib::pass_sec& userSec, 
            type::SaltFilter filter) {
        std::stringstream qry;
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        qry << "SELECT * FROM Salt WHERE ";

        MYSQL_BIND params[1];
        std::memset(params, 0, sizeof(params));

        switch (filter) {
            case type::SaltFilter::userId:
                qry << "UserId = ?";

                params[0].buffer_type = MYSQL_TYPE_LONG;
                params[0].buffer = (char*)&userSec.user_id;
                params[0].length = 0;
                params[0].is_null = 0;
                break;
            default:
                break;
        }

        qry << " LIMIT 1";

        const auto query = qry.str();
        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, params);
        status = mysql_stmt_execute(stmt);

        userSec = parseSaltRecord(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);

        return userSec;
    }


    bool UserRepository::doesUserRecordExist(const icarus_lib::user& user, type::UserFilter filter) {
        std::stringstream qry;
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        qry << "SELECT * FROM User WHERE ";

        MYSQL_BIND params[1];
        std::memset(params, 0, sizeof(params));

        auto userLength = user.username.size();
        switch (filter) {
            case type::UserFilter::username:
                qry << "Username = ?";

                params[0].buffer_type = MYSQL_TYPE_STRING;
                params[0].buffer = (char*)user.username.c_str();
                params[0].length = &userLength;
                params[0].is_null = 0;
                break;
            default:
                break;
        }

        qry << " LIMIT 1";

        const auto query = qry.str();
        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, params);
        status = mysql_stmt_execute(stmt);

        mysql_stmt_store_result(stmt);
        const auto rowCount = mysql_stmt_num_rows(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);

        return (rowCount > 0) ? true : false;
    }


    void UserRepository::saveUserRecord(const icarus_lib::user& user) {
        std::cout << "inserting user record\n";
        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        std::stringstream qry;
        qry << "INSERT INTO User(Firstname, Lastname, Phone, Email, Username, Password) ";
        qry << "VALUES(?, ?, ?, ?, ?, ?)";

        const auto query = qry.str();
        auto sizes = fetchUserLengths(user);
        auto params = insertUserValues(user, sizes);

        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, params.get());
        status = mysql_stmt_execute(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);
    }

    void UserRepository::saveUserSalt(const icarus_lib::pass_sec& userSec) {
        std::cout << "inserting user salt record\n";

        auto conn = setupMysqlConnection();
        auto stmt = mysql_stmt_init(conn);

        std::stringstream qry;
        qry << "INSERT INTO Salt(Salt, UserId) VALUES(?,?)";

        const auto query = qry.str();

        auto sizes = fetchSaltLengths(userSec);
        auto values = insertSaltValues(userSec, sizes);

        auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
        status = mysql_stmt_bind_param(stmt, values.get());
        status = mysql_stmt_execute(stmt);

        mysql_stmt_close(stmt);
        mysql_close(conn);
    }


    std::shared_ptr<MYSQL_BIND> UserRepository::insertUserValues(const icarus_lib::user& user, 
            std::shared_ptr<UserRepository::UserLengths> lengths) {
        std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(6, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_STRING;
        values.get()[0].buffer = (char*)user.firstname.c_str();
        values.get()[0].length = &(lengths->firstnameLength);
        values.get()[0].is_null = 0;

        values.get()[1].buffer_type = MYSQL_TYPE_STRING;
        values.get()[1].buffer = (char*)user.lastname.c_str();
        values.get()[1].length = &(lengths->lastnameLength);
        values.get()[1].is_null = 0;

        values.get()[2].buffer_type = MYSQL_TYPE_STRING;
        values.get()[2].buffer = (char*)user.phone.c_str();
        values.get()[2].length = &(lengths->phoneLength);
        values.get()[2].is_null = 0;
    
        values.get()[3].buffer_type = MYSQL_TYPE_STRING;
        values.get()[3].buffer = (char*)user.email.c_str();
        values.get()[3].length = &(lengths->emailLength);
        values.get()[3].is_null = 0;
    
        values.get()[4].buffer_type = MYSQL_TYPE_STRING;
        values.get()[4].buffer = (char*)user.username.c_str();
        values.get()[4].length = &(lengths->usernameLength);
        values.get()[4].is_null = 0;
    
        values.get()[5].buffer_type = MYSQL_TYPE_STRING;
        values.get()[5].buffer = (char*)user.password.c_str();
        values.get()[5].length = &(lengths->passwordLength);
        values.get()[5].is_null = 0;
    
        return values;
    }

    std::shared_ptr<MYSQL_BIND> UserRepository::insertSaltValues(const icarus_lib::pass_sec& passSec,
            std::shared_ptr<UserRepository::SaltLengths> lengths) {
        std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(6, sizeof(MYSQL_BIND)));

        values.get()[0].buffer_type = MYSQL_TYPE_STRING;
        values.get()[0].buffer = (char*)passSec.hash_password.c_str();
        values.get()[0].length = &(lengths->saltLength);

        values.get()[1].buffer_type = MYSQL_TYPE_LONG;
        values.get()[1].buffer = (char*)&passSec.user_id;

        return values;
    }

    std::shared_ptr<MYSQL_BIND> UserRepository::valueBind(icarus_lib::user& user,
            std::tuple<char*, char*, char*, char*, char*, char*>& us) {
        std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(7, sizeof(MYSQL_BIND)));
        constexpr auto strLen = 1024;

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = (char*)&user.id;

        values.get()[1].buffer_type = MYSQL_TYPE_STRING;
        values.get()[1].buffer = (char*)std::get<0>(us);
        values.get()[1].buffer_length = strLen;

        values.get()[2].buffer_type = MYSQL_TYPE_STRING;
        values.get()[2].buffer = (char*)std::get<1>(us);
        values.get()[2].buffer_length = strLen;

        values.get()[3].buffer_type = MYSQL_TYPE_STRING;
        values.get()[3].buffer = (char*)std::get<2>(us);
        values.get()[3].buffer_length = strLen;
    
        values.get()[4].buffer_type = MYSQL_TYPE_STRING;
        values.get()[4].buffer = (char*)std::get<3>(us);
        values.get()[4].buffer_length = strLen;
    
        values.get()[5].buffer_type = MYSQL_TYPE_STRING;
        values.get()[5].buffer = (char*)std::get<4>(us);
        values.get()[5].buffer_length = strLen;
    
        values.get()[6].buffer_type = MYSQL_TYPE_STRING;
        values.get()[6].buffer = (char*)std::get<5>(us);
        values.get()[6].buffer_length = strLen;

        return values;
    }

    std::shared_ptr<MYSQL_BIND> UserRepository::saltValueBind(icarus_lib::pass_sec& userSalt, 
            char *salt) {
        std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(3, sizeof(MYSQL_BIND)));
        constexpr auto strLen = 1024;

        values.get()[0].buffer_type = MYSQL_TYPE_LONG;
        values.get()[0].buffer = (char*)&userSalt.id;

        values.get()[1].buffer_type = MYSQL_TYPE_STRING;
        values.get()[1].buffer = (char*)salt;
        values.get()[1].buffer_length = strLen;

        values.get()[2].buffer_type = MYSQL_TYPE_LONG;
        values.get()[2].buffer = (char*)&userSalt.user_id;

        return values;
    }

    std::shared_ptr<UserRepository::UserLengths> UserRepository::fetchUserLengths(
            const icarus_lib::user& user) {
        auto userLen = std::make_shared<UserLengths>();
        userLen->firstnameLength = user.firstname.size();
        userLen->lastnameLength = user.lastname.size();
        userLen->phoneLength = user.phone.size();
        userLen->emailLength = user.email.size();
        userLen->usernameLength = user.username.size();
        userLen->passwordLength = user.password.size();

        return userLen;
    }

    std::shared_ptr<UserRepository::SaltLengths> UserRepository::fetchSaltLengths(
            const icarus_lib::pass_sec& passSec) {
        auto saltLen = std::make_shared<SaltLengths>();
        saltLen->saltLength = passSec.salt.size();

        return saltLen;
    }


    std::tuple<char*, char*, char*, char*, char*, char*> UserRepository::fetchUV() {
        char firstname[1024];
        char lastname[1024];
        char phone[1024];
        char email[1024];
        char username[1024];
        char password[1024];

        return std::make_tuple(firstname, lastname, phone, email, username, password);
    }


    icarus_lib::user UserRepository::parseRecord(MYSQL_STMT *stmt) {
        icarus_lib::user user;
        auto usv = fetchUV();
    
        auto bindedValues = valueBind(user, usv);
        auto status = mysql_stmt_bind_result(stmt, bindedValues.get());
        status = mysql_stmt_fetch(stmt);

        user.firstname = std::get<0>(usv);
        user.lastname = std::get<1>(usv);
        user.email = std::get<2>(usv);
        user.phone = std::get<3>(usv);
        user.username = std::get<4>(usv);
        user.password = std::get<5>(usv);

        return user;
    }

    icarus_lib::pass_sec UserRepository::parseSaltRecord(MYSQL_STMT* stmt) {
        icarus_lib::pass_sec userSalt;
        char saltKey[1024];

        auto bindedValues = saltValueBind(userSalt, saltKey);
        auto status = mysql_stmt_bind_result(stmt, bindedValues.get());
        status = mysql_stmt_fetch(stmt);

        userSalt.salt = saltKey;

        return userSalt;
    }
}
