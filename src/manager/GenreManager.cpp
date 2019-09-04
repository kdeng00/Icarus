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

void manager::GenreManager::printGenre(const model::Genre& genre)
{
    std::cout << "genre record" << std::endl;
    std::cout << "id: " << genre.id << std::endl;
    std::cout << "category: " << genre.category << std::endl;
}
