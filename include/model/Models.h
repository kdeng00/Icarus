#ifndef MODELS_H_
#define MODELS_H_

#include <string>
#include <vector>

namespace model
{
    struct Song
    {
        Song() = default;
        Song(const int id) : id(id) { }

        int id; 
        std::string title;
        std::string artist;
        std::string album;
        std::string genre;
        int year;
        int duration;
        int track;
        int disc;
        std::string songPath;
        std::vector<unsigned char> data;
        int coverArtId;
        int artistId;
        int albumId;
        int genreId;
        int yearId;
    };

    struct Artist
    {
        Artist() = default;
        Artist(const int id) : id(id) { }

        int id;
        std::string artist;
    };

    struct Album
    {
        Album() = default;
        Album(const int id) : id(id) { }

        int id;
        std::string title;
        int year;
        std::vector<Song> songs;
    };

    struct Genre
    {
        Genre() = default;
        Genre(const int id) : id(id) { }

        int id;
        std::string category;

    };

    struct Year
    {
        Year() = default;
        Year(const int id) : id(id) { }

        int id;
        int year;
    };

    struct Cover
    {
        Cover() = default;
        Cover(const int id) : id(id) { }

        int id;
        std::string songTitle;
        std::string imagePath;
        // Not being used but it should be
        std::vector<unsigned char> data;
    };

    struct LoginResult
    {
        int userId;
        std::string username;
        std::string accessToken;
        std::string tokenType;
        std::string message;
        int expiration;
    };

    struct AuthCredentials
    {
        std::string domain;
        std::string apiIdentifier;
        std::string clientId;
        std::string clientSecret;
        std::string uri;
        std::string endpoint;
    };

    struct DatabaseConnection
    {
        std::string server;
        std::string username;
        std::string password;
        std::string database;
    };

    struct BinaryPath
    {
        BinaryPath() = default;
        BinaryPath(const char *p) : path(std::move(p)) { }
        BinaryPath(std::string& p) : path(p) { }
        BinaryPath(const std::string& p) : path(p) { }

        std::string path;
    };
}

#endif
