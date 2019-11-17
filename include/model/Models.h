#ifndef MODELS_H_
#define MODELS_H_

#include <string>
#include <vector>

namespace model {
    class Song {
    public:
        Song() = default;
        Song(const int id) : id(id) { }
        Song(const int id, const std::string& title, const std::string& artist,
                const std::string& album, const std::string& albumArtist,
                const std::string& genre, const int year, const int duration,
                const int track, const int disc, const std::string songPath) :
                id(id), title(title), artist(artist), album(album),
                albumArtist(albumArtist), genre(genre), year(year),
                duration(duration), track(track), disc(disc), 
                songPath(songPath) { }

        int id; 
        std::string title;
        std::string artist;
        std::string album;
        std::string albumArtist;
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

    class Artist {
    public:
        Artist() = default;
        Artist(const Song& song) : id(song.artistId), artist(song.artist) { }
        Artist(const int id) : id(id) { }

        int id;
        std::string artist;
    };

    class Album {
    public:
        Album() = default;
        Album(const Song& song) : 
                id(song.albumId), title(song.album),artist(song.artist), year(song.year) { }
        Album(const int id) : id(id) { }

        int id;
        std::string title;
        std::string artist;
        int year;
        std::vector<Song> songs;
    };

    class Genre {
    public:
        Genre() = default;
        Genre(const Song& song) : 
                id(song.genreId), category(song.genre) { }
        Genre(const int id) : id(id) { }

        int id;
        std::string category;

    };

    class Year {
    public:
        Year() = default;
        Year(const Song& song) : 
                id(song.yearId), year(song.year) { }
        Year(const int id) : id(id) { }

        int id;
        int year;
    };

    class Cover {
    public:
        Cover() = default;
        Cover(const Song& song) : 
                id(song.coverArtId), songTitle(song.title) { }
        Cover(const int id) : id(id) { }

        int id;
        std::string songTitle;
        std::string imagePath;
        // Not being used but it should be
        std::vector<unsigned char> data;
    };

    class Token {
    public:
        Token() = default;
        Token(const std::string& accessToken) :
                accessToken(accessToken) { }
        Token(const std::string& accessToken, const std::string& tokenType, 
                const int expiration) :
                accessToken(accessToken), tokenType(tokenType), 
                expiration(expiration) { }

        std::string accessToken;
        int expiration;
        std::string tokenType;
    };

    struct LoginResult {
        int userId;
        std::string username;
        std::string accessToken;
        std::string tokenType;
        std::string message;
        int expiration;
    };

    class RegisterResult {
    public:
        std::string username;
        bool registered;
        std::string message;
    };

    struct User {
        int id;
        std::string firstname;
        std::string lastname;
        std::string email;
        std::string phone;
        std::string username;
        std::string password;
    };

    struct PassSec {
        int id;
        std::string hashPassword;
        std::string salt;
        int userId;
    };

    struct AuthCredentials {
        std::string domain;
        std::string apiIdentifier;
        std::string clientId;
        std::string clientSecret;
        std::string uri;
        std::string endpoint;
    };

    struct DatabaseConnection {
        std::string server;
        std::string username;
        std::string password;
        std::string database;
    };

    class BinaryPath {
    public:
        BinaryPath() = default;
        BinaryPath(const char *p) : path(p) { }
        BinaryPath(const std::string& p) : path(p) { }
        BinaryPath(const std::string&& p) : path(p) { }

        std::string path;
    };
}

#endif
