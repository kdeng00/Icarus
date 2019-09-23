#include "database/UserRepository.h"

#include <string>
#include <sstream>
#include <cstring>

namespace database {

UserRepository::UserRepository(const model::BinaryPath& bConf) : BaseRepository(bConf) { }


model::User UserRepository::retrieveUserRecord(model::User& user, 
                                              type::UserFilter filter = type::UserFilter::username)
{
    std::stringstream qry;
    auto conn = setupMysqlConnection();
    auto stmt = mysql_stmt_init(conn);

    qry << "SELECT * FROM User WHERE ";

    MYSQL_BIND params[1];
    std::memset(params, 0, sizeof(params));

    auto userLength = user.username.size();
    switch (filter) {
        case type::UserFilter::id:
            break;
        case type::UserFilter::username:
            qry << "Username = ?";

            params[0].buffer_type = MYSQL_TYPE_LONG;
            params[0].buffer = (char*)user.username.c_str();
            params[0].length = &userLength;
            params[0].is_null = 0;
            break;
        default:
            break;
    }

    const auto query = qry.str();
    auto status = mysql_stmt_prepare(stmt, query.c_str(), query.size());
    status = mysql_stmt_bind_param(stmt, params);
    status = mysql_stmt_execute(stmt);

    user = parseRecord(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);

    return user;
}


void UserRepository::saveUserRecord(const model::User& user)
{
    std::cout << "inserting user record" << std::endl;
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


std::shared_ptr<MYSQL_BIND> UserRepository::insertUserValues(const model::User& user, 
        std::shared_ptr<UserRepository::UserLengths> lengths)
{
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

std::shared_ptr<MYSQL_BIND> UserRepository::valueBind(model::User& user, std::shared_ptr<UserVals> userVal)
{
    std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(7, sizeof(MYSQL_BIND)));
    constexpr auto strLen = 1024;

    values.get()[0].buffer_type = MYSQL_TYPE_LONG;
    values.get()[0].buffer = (char*)&user.id;

    values.get()[1].buffer_type = MYSQL_TYPE_STRING;
    values.get()[1].buffer = (char*)userVal->firstname;
    values.get()[1].buffer_length = strLen;

    values.get()[2].buffer_type = MYSQL_TYPE_STRING;
    values.get()[2].buffer = (char*)userVal->lastname;
    values.get()[2].buffer_length = strLen;

    values.get()[3].buffer_type = MYSQL_TYPE_STRING;
    values.get()[3].buffer = (char*)userVal->phone;
    values.get()[3].buffer_length = strLen;
    
    values.get()[4].buffer_type = MYSQL_TYPE_STRING;
    values.get()[4].buffer = (char*)userVal->email;
    values.get()[4].buffer_length = strLen;
    
    values.get()[5].buffer_type = MYSQL_TYPE_STRING;
    values.get()[5].buffer = (char*)userVal->username;
    values.get()[5].buffer_length = strLen;
    
    values.get()[6].buffer_type = MYSQL_TYPE_STRING;
    values.get()[6].buffer = (char*)userVal->password;
    values.get()[6].buffer_length = strLen;

    return values;
}

std::shared_ptr<MYSQL_BIND> UserRepository::valueBind(model::User& user, UserVals& userVal)
{
    std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(7, sizeof(MYSQL_BIND)));
    constexpr auto strLen = 1024;

    values.get()[0].buffer_type = MYSQL_TYPE_LONG;
    values.get()[0].buffer = (char*)&user.id;

    values.get()[1].buffer_type = MYSQL_TYPE_STRING;
    values.get()[1].buffer = (char*)userVal.firstname;
    values.get()[1].buffer_length = strLen;

    values.get()[2].buffer_type = MYSQL_TYPE_STRING;
    values.get()[2].buffer = (char*)userVal.lastname;
    values.get()[2].buffer_length = strLen;

    values.get()[3].buffer_type = MYSQL_TYPE_STRING;
    values.get()[3].buffer = (char*)userVal.phone;
    values.get()[3].buffer_length = strLen;
    
    values.get()[4].buffer_type = MYSQL_TYPE_STRING;
    values.get()[4].buffer = (char*)userVal.email;
    values.get()[4].buffer_length = strLen;
    
    values.get()[5].buffer_type = MYSQL_TYPE_STRING;
    values.get()[5].buffer = (char*)userVal.username;
    values.get()[5].buffer_length = strLen;
    
    values.get()[6].buffer_type = MYSQL_TYPE_STRING;
    values.get()[6].buffer = (char*)userVal.password;
    values.get()[6].buffer_length = strLen;

    return values;
}

std::shared_ptr<MYSQL_BIND> UserRepository::valueBind(model::User& user)
{
    std::shared_ptr<MYSQL_BIND> values((MYSQL_BIND*) std::calloc(7, sizeof(MYSQL_BIND)));
    constexpr auto strLen = 1024;

    values.get()[0].buffer_type = MYSQL_TYPE_LONG;
    values.get()[0].buffer = (char*)&user.id;

    values.get()[1].buffer_type = MYSQL_TYPE_STRING;
    values.get()[1].buffer = (char*)user.firstname.data();
    values.get()[1].buffer_length = strLen;

    values.get()[2].buffer_type = MYSQL_TYPE_STRING;
    values.get()[2].buffer = (char*)user.lastname.data();
    values.get()[2].buffer_length = strLen;

    values.get()[3].buffer_type = MYSQL_TYPE_STRING;
    values.get()[3].buffer = (char*)user.phone.data();
    values.get()[3].buffer_length = strLen;
    
    values.get()[4].buffer_type = MYSQL_TYPE_STRING;
    values.get()[4].buffer = (char*)user.email.data();
    values.get()[4].buffer_length = strLen;
    
    values.get()[5].buffer_type = MYSQL_TYPE_STRING;
    values.get()[5].buffer = (char*)user.username.data();
    values.get()[5].buffer_length = strLen;
    
    values.get()[6].buffer_type = MYSQL_TYPE_STRING;
    values.get()[6].buffer = (char*)user.password.data();
    values.get()[6].buffer_length = strLen;

    return values;
}

std::shared_ptr<UserRepository::UserLengths> UserRepository::fetchUserLengths(const model::User& user)
{
    auto userLen = std::make_shared<UserLengths>();
    userLen->firstnameLength = user.firstname.size();
    userLen->lastnameLength = user.lastname.size();
    userLen->phoneLength = user.phone.size();
    userLen->emailLength = user.email.size();
    userLen->usernameLength = user.username.size();
    userLen->passwordLength = user.password.size();

    return userLen;
}

std::shared_ptr<UserRepository::UserVals> UserRepository::fetchUserVals()
{
    constexpr auto strLen = 1024;
    auto userVal = std::make_shared<UserVals>();
    /**
    userVal->firstname = std::make_shared<char*>(new char[strLen]);
    userVal->lastname = std::make_shared<char*>(new char[strLen]);
    userVal->email = std::make_shared<char*>(new char[strLen]);
    userVal->phone = std::make_shared<char*>(new char[strLen]);
    userVal->username = std::make_shared<char*>(new char[strLen]);
    userVal->password = std::make_shared<char*>(new char[strLen]);
    */

    return userVal;
}


model::User UserRepository::parseRecord(MYSQL_STMT *stmt)
{
    // TODO : parse user record
    model::User user;
    auto userVal = fetchUserVals();
    UserVals uv;
    //auto bindedValues = valueBind(user, userVal);
    auto bindedValues = valueBind(user, uv);
    //auto bindedValues = valueBind(user);
    auto status = mysql_stmt_bind_result(stmt, bindedValues.get());
    status = mysql_stmt_fetch(stmt);

    std::cout << user.id << std::endl;
    //std::cout << "binded firstname: " << userVal->firstname << std::endl;
    //std::cout << "binded lastname: " << uv.lastname << std::endl;
    //user.firstname = userVal->firstname;
    /**
    user.lastname = *userVal->lastname.get();
    user.email = *userVal->email.get();
    user.phone = *userVal->phone.get();
    user.username = *userVal->username.get();
    user.password = *userVal->password.get();
    */

    return user;
}
}
