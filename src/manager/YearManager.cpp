#include "manager/YearManager.h"

#include <iostream>

#include "database/YearRepository.h"
#include "type/YearFilter.h"

manager::YearManager::YearManager(const model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


model::Year manager::YearManager::retrieveYear(model::Year& year)
{
    database::YearRepository yearRepo(m_bConf);
    year = yearRepo.retrieveRecord(year, type::YearFilter::year);

    return year;
}

model::Year manager::YearManager::saveYear(const model::Song& song)
{
    model::Year year;
    year.year = song.year;

    database::YearRepository yearRepo(m_bConf);
    if (!yearRepo.doesYearExist(year, type::YearFilter::year)) {
        yearRepo.saveRecord(year);
    } else {
        std::cout << "year record already exists in the database" << std::endl;
    }

    return year;
}

void manager::YearManager::deleteYear(const model::Song& song)
{
    model::Year year(song);

    database::YearRepository yrRepo(m_bConf);
    auto yrWSC = yrRepo.retrieveRecordWithSongCount(year, type::YearFilter::id);

    if (yrWSC.second > 1) {
        std::cout << "year still contain songs related to it";
        std::cout << ", will not delete" << std::endl;
        return;
    }

    std::cout << "safe to delete the year record" << std::endl;
    yrRepo.deleteYear(year, type::YearFilter::id);
}

void manager::YearManager::printYear(const model::Year& year)
{
    std::cout << "\nyear record" << std::endl;
    std::cout << "id: " << year.id << std::endl;
    std::cout << "year: " << year.year << std::endl;
}
