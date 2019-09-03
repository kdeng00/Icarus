#include "managers/artistManager.h"

#include <iostream>

#include "database/artistRepository.h"
#include "types/artistFilter.h"

Manager::artistManager::artistManager(const Model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


Model::Artist Manager::artistManager::retrieveArtist(Model::Artist& artist)
{
    Database::artistRepository artRepo(m_bConf);
    artist = artRepo.retrieveRecord(artist, Type::artistFilter::artist);

    return artist;
}

Model::Artist Manager::artistManager::saveArtist(const Model::Song& song)
{
    Model::Artist artist;
    artist.artist = song.artist;

    Database::artistRepository artRepo(m_bConf);
    if (!artRepo.doesArtistExist(artist, Type::artistFilter::artist)) {
        artRepo.saveRecord(artist);
    } else {
        std::cout << "artist already exists" << std::endl;
    }

    return artist;
}

void Manager::artistManager::printArtist(const Model::Artist& artist)
{
    std::cout << "\nartist record" << std::endl;
    std::cout << "id: " << artist.id << std::endl;
    std::cout << "artist: " << artist.artist << std::endl;
}
