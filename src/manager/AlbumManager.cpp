#include "manager/AlbumManager.h"

#include <iostream>

#include "database/AlbumRepository.h"
#include "model/Models.h"
#include "type/AlbumFilter.h"

namespace manager {
AlbumManager::AlbumManager(const model::BinaryPath& bConf) : m_bConf(bConf) { }


model::Album AlbumManager::retrieveAlbum(model::Album& album)
{
    database::AlbumRepository albRepo(m_bConf);
    album = std::move(albRepo.retrieveRecord(album, type::AlbumFilter::title));

    return album;
}

model::Album AlbumManager::saveAlbum(const model::Song& song)
{
    model::Album album(song);
    
    database::AlbumRepository albRepo(m_bConf);
    // TODO: check for existence with the title and the artist
    if (!albRepo.doesAlbumExists(album, type::AlbumFilter::title)) {
        albRepo.saveAlbum(album);
    } else {
        std::cout << "album record already exists in the database" << std::endl;
    }


    return album;
}


void AlbumManager::deleteAlbum(const model::Song& song)
{
    model::Album album(song);

    database::AlbumRepository albRepo(m_bConf);
    auto albWSC = albRepo.retrieveRecordWithSongCount(album, type::AlbumFilter::id);
    
    if (albWSC.second > 1) {
        std::cout << "album still contain songs related to it, will not delete" << std::endl;
        return;
    }

    std::cout << "safe to delete the album record" << std::endl;
    albRepo.deleteAlbum(album, type::AlbumFilter::id);
}

void AlbumManager::updateAlbum(model::Song& updatedSong,
    const model::Song& currSong)
{
    model::Album album(updatedSong);

    database::AlbumRepository albRepo(m_bConf);
    if (!albRepo.doesAlbumExists(album, type::AlbumFilter::title)) {
        std::cout << "album record does not exist" << std::endl;
        albRepo.saveAlbum(album);
    } else {
        std::cout << "album record already exists" << std::endl;
    }

    album = albRepo.retrieveRecord(album, type::AlbumFilter::title);
    updatedSong.albumId = album.id;
}

void AlbumManager::printAlbum(const model::Album& album)
{
    std::cout << "\nalbum record" << std::endl;
    std::cout << "id: " << album.id << std::endl;
    std::cout << "title: " << album.title << std::endl;
    std::cout << "year: " << album.year << std::endl;
}
}
