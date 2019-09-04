#include "manager/AlbumManager.h"

#include <iostream>

#include "database/AlbumRepository.h"
#include "model/Models.h"
#include "type/AlbumFilter.h"

manager::AlbumManager::AlbumManager(const model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


model::Album manager::AlbumManager::retrieveAlbum(model::Album& album)
{
    database::AlbumRepository albRepo(m_bConf);
    album = std::move(albRepo.retrieveRecord(album, type::AlbumFilter::title));

    return album;
}

model::Album manager::AlbumManager::saveAlbum(const model::Song& song)
{
    model::Album album;
    album.title = song.album;
    album.year = song.year;
    
    database::AlbumRepository albRepo(m_bConf);
    if (!albRepo.doesAlbumExists(album, type::AlbumFilter::title)) {
        albRepo.saveAlbum(album);
    } else {
        std::cout << "album record already exists in the database" << std::endl;
    }


    return album;
}

void manager::AlbumManager::printAlbum(const model::Album& album)
{
    std::cout << "\nalbum record" << std::endl;
    std::cout << "id: " << album.id << std::endl;
    std::cout << "title: " << album.title << std::endl;
    std::cout << "year: " << album.year << std::endl;
}
