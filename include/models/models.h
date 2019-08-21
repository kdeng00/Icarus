#ifndef MODELS_H_
#define MODELS_H_

#include <string>
#include <vector>

struct Song
{
    int id;
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    int year;
    int duration;
    std::string songPath;
    std::vector<unsigned char> data;
};

struct Cover
{
    int id;
    std::string songTitle;
    std::string imagePath;
    // Not being used but it should be
    std::vector<unsigned char> data;
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
