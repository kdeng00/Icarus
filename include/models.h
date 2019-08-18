#ifndef MODELS_H_
#define MODELS_H_

#include <string>

struct Song
{
    int Id;
    char Title[1024];
    char Artist[1024];
    char Album[1024];
    char Genre[1024];
    int Year;
    int Duration;
    char SongPath[1024];
};

struct Cover
{
    int Id;
    char SongTitle[1024];
    char ImagePath[1024];
};

struct LoginRes
{
    int UserId;
    char Username[1024];
    char Token[1024];
    char TokenType[1024];
    char Message[1024];
    int Expiration;
};

struct loginResult
{
    int user_id;
    std::string username;
    std::string access_token;
    std::string token_type;
    std::string message;
    int expiration;
};

struct auth_credentials
{
    std::string domain;
    std::string api_identifier;
    std::string client_id;
    std::string client_secret;
    std::string uri;
    std::string endpoint;
};

struct database_connection
{
    std::string server;
    std::string username;
    std::string password;
    std::string database;
};

struct TokenReq
{
    char ClientId[1024];
    char ClientSecret[1024];
    char Audience[1024];
    char GrantType[1024];
    char URI[1024];
    char Endpoint[1024];
};

#endif
