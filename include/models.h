#ifndef MODELS_H_
#define MODELS_H_

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
