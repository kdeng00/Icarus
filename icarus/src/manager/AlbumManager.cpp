#include "manager/AlbumManager.h"

#include <iostream>

#include "icarus_lib/icarus.h"

#include "database/AlbumRepository.h"
#include "type/AlbumFilter.h"

namespace manager {
    AlbumManager::AlbumManager(const icarus_lib::binary_path& bConf) : m_bConf(bConf) { }


    icarus_lib::album AlbumManager::retrieveAlbum(icarus_lib::album& album) {
        database::AlbumRepository albRepo(m_bConf);
        album = std::move(albRepo.retrieveRecord(album, type::AlbumFilter::title));

        return album;
    }

    icarus_lib::album AlbumManager::saveAlbum(const icarus_lib::song& song) {
        icarus_lib::album album(song);
    
        database::AlbumRepository albRepo(m_bConf);
        // TODO: check for existence with the title and the artist
        if (!albRepo.doesAlbumExists(album, type::AlbumFilter::title)) {
            albRepo.saveAlbum(album);
        } else {
            std::cout << "album record already exists in the database\n";
        }

        return album;
    }


    void AlbumManager::deleteAlbum(const icarus_lib::song& song) {
        icarus_lib::album album(song);

        database::AlbumRepository albRepo(m_bConf);
        auto albWSC = albRepo.retrieveRecordWithSongCount(album, type::AlbumFilter::id);
    
        if (albWSC.second > 1) {
            std::cout << "album still contain songs related to it, will not delete\n";
            return;
        }

        std::cout << "safe to delete the album record\n";
        albRepo.deleteAlbum(album, type::AlbumFilter::id);
    }

    void AlbumManager::updateAlbum(icarus_lib::song& updatedSong,
            const icarus_lib::song& currSong) {
        icarus_lib::album album(updatedSong);

        database::AlbumRepository albRepo(m_bConf);
        if (!albRepo.doesAlbumExists(album, type::AlbumFilter::title)) {
            std::cout << "album record does not exist\n";
            albRepo.saveAlbum(album);
        } else {
            std::cout << "album record already exists\n";
        }

        album = albRepo.retrieveRecord(album, type::AlbumFilter::title);
        updatedSong.album_id = album.id;
    }

    void AlbumManager::printAlbum(const icarus_lib::album& album) {
        std::cout << "\nalbum record\n";
        std::cout << "id: " << album.id << "\n";
        std::cout << "title: " << album.title << "\n";
        std::cout << "year: " << album.year << "\n";
    }
}
