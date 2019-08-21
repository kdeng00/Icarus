CREATE DATABASE Icarus;

USE Icarus;

CREATE TABLE CoverArt (
    CoverArtId INT NOT NULL AUTO_INCREMENT,
    SongTitle TEXT NOT NULL,
    ImagePath TEXT NOT NULL,

    PRIMARY KEY (CoverArtId)
);

CREATE TABLE Song (
    SongId INT NOT NULL AUTO_INCREMENT,
    Title TEXT NOT NULL,
    Artist TEXT NOT NULL,
    Album TEXT NOT NULL,
    Genre TEXT NOT NULL,
    Year INT NOT NULL,
    Duration INT NOT NULL,
    CoverArtId INT NOT NULL,

    PRIMARY KEY (SongId),
    CONSTRAINT FK_CoverArtId FOREIGN KEY (CoverArtId) REFERENCES CoverArt(CoverArtId)
);

CREATE TABLE User (
    UserId INT NOT NULL AUTO_INCREMENT,
    Username TEXT NOT NULL,
    Password TEXT NOT NULL,

    PRIMARY KEY (UserId)
);
