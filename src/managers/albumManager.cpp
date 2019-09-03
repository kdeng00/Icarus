#include "managers/albumManager.h"

#include <iostream>

#include "database/albumRepository.h"
#include "models/models.h"
#include "types/albumFilter.h"

Manager::albumManager::albumManager(const Model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


Model::Album Manager::albumManager::retrieveAlbum(Model::Album& album)
{
    Database::albumRepository albRepo(m_bConf);
    album = std::move(albRepo.retrieveRecord(album, Type::albumFilter::title));

    return album;
}

Model::Album Manager::albumManager::saveAlbum(const Model::Song& song)
{
    Model::Album album;
    album.title = song.album;
    album.year = song.year;
    
    Database::albumRepository albRepo(m_bConf);
    if (!albRepo.doesAlbumExists(album, Type::albumFilter::title)) {
        albRepo.saveAlbum(album);
    } else {
        std::cout << "album record already exists in the database" << std::endl;
    }


    return album;
}

void Manager::albumManager::printAlbum(const Model::Album& album)
{
    std::cout << "\nalbum record" << std::endl;
    std::cout << "id: " << album.id << std::endl;
    std::cout << "title: " << album.title << std::endl;
    std::cout << "year: " << album.year << std::endl;
}
