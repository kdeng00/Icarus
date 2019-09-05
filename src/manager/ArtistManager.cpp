#include "manager/ArtistManager.h"

#include <iostream>

#include "database/ArtistRepository.h"
#include "type/ArtistFilter.h"

manager::ArtistManager::ArtistManager(const model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


model::Artist manager::ArtistManager::retrieveArtist(model::Artist& artist)
{
    std::cout << "retrieving artist record" << std::endl;
    database::ArtistRepository artRepo(m_bConf);
    std::cout << "initialized artist repo" << std::endl;
    std::cout << artist.artist << std::endl;
    artist = artRepo.retrieveRecord(artist, type::ArtistFilter::artist);

    return artist;
}

model::Artist manager::ArtistManager::saveArtist(const model::Song& song)
{
    model::Artist artist;
    artist.artist = song.artist;

    database::ArtistRepository artRepo(m_bConf);
    if (!artRepo.doesArtistExist(artist, type::ArtistFilter::artist)) {
        artRepo.saveRecord(artist);
    } else {
        std::cout << "artist already exists" << std::endl;
    }

    return artist;
}

void manager::ArtistManager::printArtist(const model::Artist& artist)
{
    std::cout << "\nartist record" << std::endl;
    std::cout << "id: " << artist.id << std::endl;
    std::cout << "artist: " << artist.artist << std::endl;
}
