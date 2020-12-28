#include "manager/ArtistManager.h"

#include <iostream>

#include "database/ArtistRepository.h"
#include "type/ArtistFilter.h"

namespace manager {

    ArtistManager::ArtistManager(const icarus_lib::binary_path & bConf) : m_bConf(bConf) { }


    model::Artist ArtistManager::retrieveArtist(model::Artist& artist) {
        std::cout << "retrieving artist record\n";
        database::ArtistRepository artRepo(m_bConf);
        std::cout << "initialized artist repo\n";
        std::cout << artist.artist << "\n";
        artist = artRepo.retrieveRecord(artist, type::ArtistFilter::artist);

        return artist;
    }

    model::Artist ArtistManager::saveArtist(const model::Song& song) {
        model::Artist artist;
        artist.artist = song.artist;

        database::ArtistRepository artRepo(m_bConf);
        if (!artRepo.doesArtistExist(artist, type::ArtistFilter::artist)) {
            artRepo.saveRecord(artist);
        } else {
            std::cout << "artist already exists\n";
        }

        return artist;
    }


    void ArtistManager::deleteArtist(const model::Song& song) {
        model::Artist artist(song);

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

    void ArtistManager::updateArtist(model::Song& updatedSong,
            const model::Song& currSong) {
        model::Artist artist;
        artist.artist = updatedSong.artist;

        database::ArtistRepository artRepo(m_bConf);
        if (!artRepo.doesArtistExist(artist, type::ArtistFilter::artist)) {
            std::cout << "artist record does not exist\n";
            artRepo.saveRecord(artist);
        } else {
            std::cout << "artist record already exists\n";
        }

        artist = artRepo.retrieveRecord(artist, type::ArtistFilter::artist);
        updatedSong.artistId = artist.id;
    }

    void ArtistManager::printArtist(const model::Artist& artist) {
        std::cout << "\nartist record" << "\n";
        std::cout << "id: " << artist.id << "\n";
        std::cout << "artist: " << artist.artist << "\n";
    }
}
