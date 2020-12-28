#include "manager/YearManager.h"

#include <iostream>

#include "icarus_lib/icarus.h"

#include "database/YearRepository.h"
#include "type/YearFilter.h"

namespace manager {
    YearManager::YearManager(const icarus_lib::binary_path &bConf) : m_bConf(bConf) { }


    icarus_lib::year YearManager::retrieveYear(icarus_lib::year& year) {
		database::YearRepository yearRepo(m_bConf);
		year = yearRepo.retrieveRecord(year, type::YearFilter::year);

		return year;
    }

    icarus_lib::year YearManager::saveYear(const icarus_lib::song& song) {
		icarus_lib::year year;
		year.year = song.year;

		database::YearRepository yearRepo(m_bConf);
		if (!yearRepo.doesYearExist(year, type::YearFilter::year)) {
    		yearRepo.saveRecord(year);
		} else {
		    std::cout << "year record already exists in the database\n";
		}

		return year;
    }

    void YearManager::deleteYear(const icarus_lib::song& song) {
		icarus_lib::year year(song);

		database::YearRepository yrRepo(m_bConf);
		auto yrWSC = yrRepo.retrieveRecordWithSongCount(year, type::YearFilter::id);

		if (yrWSC.second > 1) {
		    std::cout << "year still contain songs related to it";
		    std::cout << ", will not delete\n";
		    return;
		}

		std::cout << "safe to delete the year record\n";
		yrRepo.deleteYear(year, type::YearFilter::id);
    }

    void YearManager::updateYear(icarus_lib::song& updatedSong,
		const icarus_lib::song& currSong) {
		icarus_lib::year year;
		year.year = updatedSong.year;

		database::YearRepository albRepo(m_bConf);
		if (!albRepo.doesYearExist(year, type::YearFilter::year)) {
		    std::cout << "year record does not exist\n";
		    albRepo.saveRecord(year);
		} else {
		    std::cout << "year record already exists\n";
		}

		year = albRepo.retrieveRecord(year, type::YearFilter::year);
		updatedSong.yearId = year.id;
    }

    void YearManager::printYear(const icarus_lib::year& year) {
		std::cout << "\nyear record\n";
		std::cout << "id: " << year.id << "\n";
		std::cout << "year: " << year.year << "\n";
    }
}
