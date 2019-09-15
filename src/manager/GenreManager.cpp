#include "manager/GenreManager.h"

#include <iostream>

#include "database/GenreRepository.h"
#include "type/GenreFilter.h"

manager::GenreManager::GenreManager(const model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


model::Genre manager::GenreManager::retrieveGenre(model::Genre& genre)
{
    database::GenreRepository gnrRepo(m_bConf);
    genre = gnrRepo.retrieveRecord(genre, type::GenreFilter::category);

    return genre;
}

model::Genre manager::GenreManager::saveGenre(const model::Song& song)
{
    model::Genre genre;
    genre.category = song.genre;
    
    database::GenreRepository gnrRepo(m_bConf);
    if (!gnrRepo.doesGenreExist(genre, type::GenreFilter::category)) {
        gnrRepo.saveRecord(genre);
    } else {
        std::cout << "genre record already exists" << std::endl;
    }

    return genre;
}


void manager::GenreManager::deleteGenre(const model::Song& song)
{
    model::Genre genre(song);

    database::GenreRepository gnrRepo(m_bConf);
    auto gnrWSC = gnrRepo.retrieveRecordWithSongCount(genre, type::GenreFilter::id);

    if (gnrWSC.second > 1) {
        std::cout << "genre still contain songs related to it";
        std::cout << ", will not delete" << std::endl;
        return;
    }

    std::cout << "safe to delete the genre record" << std::endl;
    gnrRepo.deleteRecord(genre, type::GenreFilter::id);
}

void manager::GenreManager::updateGenre(model::Song& updatedSong,
    const model::Song& currSong)
{
    model::Genre genre;
    genre.category = updatedSong.genre;

    database::GenreRepository gnrRepo(m_bConf);
    if (!gnrRepo.doesGenreExist(genre, type::GenreFilter::category)) {
        std::cout << "genre record does not exist" << std::endl;
        gnrRepo.saveRecord(genre);
    } else {
        std::cout << "genre record already exists" << std::endl;
    }

    genre = gnrRepo.retrieveRecord(genre, type::GenreFilter::category);
    updatedSong.genreId = genre.id;
}

void manager::GenreManager::printGenre(const model::Genre& genre)
{
    std::cout << "genre record" << std::endl;
    std::cout << "id: " << genre.id << std::endl;
    std::cout << "category: " << genre.category << std::endl;
}
