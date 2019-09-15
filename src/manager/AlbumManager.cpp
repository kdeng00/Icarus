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


void manager::AlbumManager::deleteAlbum(const model::Song& song)
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

void manager::AlbumManager::updateAlbum(model::Song& updatedSong,
    const model::Song& currSong)
{
    model::Album album;
    album.title = updatedSong.album;
    album.year = updatedSong.year;

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

void manager::AlbumManager::printAlbum(const model::Album& album)
{
    std::cout << "\nalbum record" << std::endl;
    std::cout << "id: " << album.id << std::endl;
    std::cout << "title: " << album.title << std::endl;
    std::cout << "year: " << album.year << std::endl;
}
