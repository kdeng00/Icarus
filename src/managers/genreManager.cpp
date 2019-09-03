#include "managers/genreManager.h"

#include <iostream>

#include "database/genreRepository.h"
#include "types/genreFilter.h"

Manager::genreManager::genreManager(const Model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


Model::Genre Manager::genreManager::retrieveGenre(Model::Genre& genre)
{
    Database::genreRepository gnrRepo(m_bConf);
    genre = gnrRepo.retrieveRecord(genre, Type::genreFilter::category);

    return genre;
}

Model::Genre Manager::genreManager::saveGenre(const Model::Song& song)
{
    Model::Genre genre;
    genre.category = song.genre;
    
    Database::genreRepository gnrRepo(m_bConf);
    if (!gnrRepo.doesGenreExist(genre, Type::genreFilter::category)) {
        gnrRepo.saveRecord(genre);
    } else {
        std::cout << "genre record already exists" << std::endl;
    }

    return genre;
}

void Manager::genreManager::printGenre(const Model::Genre& genre)
{
    std::cout << "genre record" << std::endl;
    std::cout << "id: " << genre.id << std::endl;
    std::cout << "category: " << genre.category << std::endl;
}
