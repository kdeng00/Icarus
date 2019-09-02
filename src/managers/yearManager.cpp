#include "managers/yearManager.h"

#include <iostream>

#include "database/yearRepository.h"
#include "types/yearFilter.h"

Manager::yearManager::yearManager(const Model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


Model::Year Manager::yearManager::retrieveYear(Model::Year& year)
{
    Database::yearRepository yearRepo(m_bConf);
    year = yearRepo.retrieveRecord(year, Type::yearFilter::year);

    return year;
}

Model::Year Manager::yearManager::saveYear(const Model::Song& song)
{
    Model::Year year;
    year.year = song.year;

    Database::yearRepository yearRepo(m_bConf);
    yearRepo.saveRecord(year);

    return year;
}

void Manager::yearManager::printYear(const Model::Year& year)
{
    std::cout << "\nyear record" << std::endl;
    std::cout << "id: " << year.id << std::endl;
    std::cout << "year: " << year.year << std::endl;
}
