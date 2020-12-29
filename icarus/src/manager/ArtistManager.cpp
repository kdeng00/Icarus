#include "manager/ArtistManager.h"

#include <iostream>

#include "database/ArtistRepository.h"
#include "type/ArtistFilter.h"

namespace manager {

    ArtistManager::ArtistManager(const icarus_lib::binary_path & bConf) : m_bConf(bConf) { }


    icarus_lib::artist ArtistManager::retrieveArtist(icarus_lib::artist& artist) {
        std::cout << "retrieving artist record\n";
        database::ArtistRepository artRepo(m_bConf);
        std::cout << "initialized artist repo\n";
        std::cout << artist.name << "\n";
        artist = artRepo.retrieveRecord(artist, type::ArtistFilter::artist);

        return artist;
    }

    icarus_lib::artist ArtistManager::saveArtist(const icarus_lib::song& song) {
        icarus_lib::artist artist;
        artist.name = song.artist;

        database::ArtistRepository artRepo(m_bConf);
        if (!artRepo.doesArtistExist(artist, type::ArtistFilter::artist)) {
            artRepo.saveRecord(artist);
        } else {
            std::cout << "artist already exists\n";
        }

        return artist;
    }


    void ArtistManager::deleteArtist(const icarus_lib::song& song) {
        icarus_lib::artist artist(song);

        database::ArtistRepository artRepo(m_bConf);
        auto artWSC = artRepo.retrieveRecordWithSongCount(artist, type::ArtistFilter::id);
    
        if (artWSC.second > 1) {
            std::cout << "artist still contain songs related to it";
            std::cout << ", not delete\n";
            return;
        }

        std::cout << "safe to delete the artist record\n";
        artRepo.deleteArtist(artist, type::ArtistFilter::id);
    }

    void ArtistManager::updateArtist(icarus_lib::song& updatedSong,
            const icarus_lib::song& currSong) {
        icarus_lib::artist artist;
        artist.name = updatedSong.artist;

        database::ArtistRepository artRepo(m_bConf);
        if (!artRepo.doesArtistExist(artist, type::ArtistFilter::artist)) {
            std::cout << "artist record does not exist\n";
            artRepo.saveRecord(artist);
        } else {
            std::cout << "artist record already exists\n";
        }

        artist = artRepo.retrieveRecord(artist, type::ArtistFilter::artist);
        updatedSong.artist_id = artist.id;
    }

    void ArtistManager::printArtist(const icarus_lib::artist& artist) {
        std::cout << "\nartist record" << "\n";
        std::cout << "id: " << artist.id << "\n";
        std::cout << "artist: " << artist.name << "\n";
    }
}