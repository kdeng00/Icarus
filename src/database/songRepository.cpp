#include "database/songRepository.h"

#include <iostream>
#include <memory>
#include <sstream>
#include <cstring>

songRepository::songRepository(const std::string& path) : base_repository(path)
{ }

Song songRepository::retrieveRecord(Song& song, songFilter)
{

    return song;
}

void songRepository::saveRecord(const Song& song)
{
    // TODO: pick up here. Save the song record to the database
}

std::vector<Song> songRepository::parseRecords(MYSQL_RES* results)
{
    std::vector<Song> songs;


    return songs;
}

Song songRepository::parseRecord(MYSQL_RES* results)
{
    Song song;

    return song;
}
