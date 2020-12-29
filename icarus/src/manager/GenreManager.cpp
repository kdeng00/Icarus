#include "manager/GenreManager.h"

#include <iostream>

#include "database/GenreRepository.h"
#include "type/GenreFilter.h"

namespace manager {
    GenreManager::GenreManager(const icarus_lib::binary_path & bConf) : m_bConf(bConf) { }


    icarus_lib::genre GenreManager::retrieveGenre(icarus_lib::genre& genre) {
		database::GenreRepository gnrRepo(m_bConf);
		genre = gnrRepo.retrieveRecord(genre, type::GenreFilter::category);

		return genre;
    }

    icarus_lib::genre GenreManager::saveGenre(const icarus_lib::song& song) {
		icarus_lib::genre genre;
		genre.category = song.genre;
		
		database::GenreRepository gnrRepo(m_bConf);
		if (!gnrRepo.doesGenreExist(genre, type::GenreFilter::category)) {
		    gnrRepo.saveRecord(genre);
		} else {
		    std::cout << "genre record already exists\n";
		}

		return genre;
    }


    void GenreManager::deleteGenre(const icarus_lib::song& song) {
		icarus_lib::genre genre(song);

		database::GenreRepository gnrRepo(m_bConf);
		auto gnrWSC = gnrRepo.retrieveRecordWithSongCount(genre, type::GenreFilter::id);

		if (gnrWSC.second > 1) {
		    std::cout << "genre still contain songs related to it";
		    std::cout << ", will not delete\n";
		    return;
		}

		std::cout << "safe to delete the genre record\n";
		gnrRepo.deleteRecord(genre, type::GenreFilter::id);
    }

    void GenreManager::updateGenre(icarus_lib::song& updatedSong,
		    const icarus_lib::song& currSong) {
		icarus_lib::genre genre;
		genre.category = updatedSong.genre;

		database::GenreRepository gnrRepo(m_bConf);
		if (!gnrRepo.doesGenreExist(genre, type::GenreFilter::category)) {
		    std::cout << "genre record does not exist\n";
		    gnrRepo.saveRecord(genre);
		} else {
		    std::cout << "genre record already exists\n";
		}

		genre = gnrRepo.retrieveRecord(genre, type::GenreFilter::category);
		updatedSong.genre_id = genre.id;
    }

    void GenreManager::printGenre(const icarus_lib::genre& genre) {
		std::cout << "genre record\n";
		std::cout << "id: " << genre.id << "\n";
		std::cout << "category: " << genre.category << "\n";
    }
}
