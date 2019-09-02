CREATE DATABASE Icarus;

USE Icarus;

CREATE TABLE CoverArt (
    CoverArtId INT NOT NULL AUTO_INCREMENT,
    SongTitle TEXT NOT NULL,
    ImagePath TEXT NOT NULL,

    PRIMARY KEY (CoverArtId)
);

CREATE TABLE Album (
    AlbumId INT NOT NULL AUTO_INCREMENT,
    Title TEXT NOT NULL,
    Year INT NOT NULL,

    PRIMARY KEY (AlbumId)
);

CREATE TABLE Artist (
    ArtistId INT NOT NULL AUTO_INCREMENT,
    Artist TEXT NOT NULL,

    PRIMARY KEY (ArtistId)
);

CREATE TABLE Genre (
    GenreId INT NOT NULL AUTO_INCREMENT,
    Category TEXT NOT NULL,

    PRIMARY KEY (GenreId)
);

CREATE TABLE Year (
    YearId INT NOT NULL AUTO_INCREMENT,
    Year INT NOT NULL,

    PRIMARY KEY (YearId)
);

CREATE TABLE Song (
    SongId INT NOT NULL AUTO_INCREMENT,
    Title TEXT NOT NULL,
    Artist TEXT NOT NULL,
    Album TEXT NOT NULL,
    Genre TEXT NOT NULL,
    Year INT NOT NULL,
    Duration INT NOT NULL,
    Track INT NOT NULL,
    Disc INT NOT NULL,
    SongPath TEXT NOT NULL,
    CoverArtId INT NOT NULL,
    ArtistId INT NOT NULL,
    AlbumId INT NOT NULL,
    GenreId INT NOT NULL,
    YearId INT NOT NULL,

    PRIMARY KEY (SongId),
    CONSTRAINT FK_CoverArtId FOREIGN KEY (CoverArtId) REFERENCES CoverArt (CoverArtId),
    CONSTRAINT FK_ArtistId FOREIGN KEY (ArtistId) REFERENCES Artist (ArtistId),
    CONSTRAINT FK_AlbumId FOREIGN KEY (AlbumId) REFERENCES Album (AlbumId),
    CONSTRAINT FK_GenreId FOREIGN KEY (GenreId) REFERENCES Genre (GenreId),
    CONSTRAINT FK_YearId FOREIGN KEY (YearId) REFERENCES Year (YearId)
);

CREATE TABLE User (
    UserId INT NOT NULL AUTO_INCREMENT,
    Firstname TEXT NOT NULL,
    Lastname TEXT NOT NULL,
    Email TEXT NOT NULL,
    Phone TEXT NOT NULL,
    Username TEXT NOT NULL,
    Password TEXT NOT NULL,

    PRIMARY KEY (UserId)
);
