#ifndef YEARREPOSITORY_H_
#define YEARREPOSITORY_H_

#include <utility>
#include <vector>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/YearFilter.h"

namespace database
{
    class YearRepository : public BaseRepository
    {
    public:
        YearRepository(const model::BinaryPath&);

        std::vector<model::Year> retrieveRecords();

        std::pair<model::Year, int> retrieveRecordWithSongCount(model::Year&, type::YearFilter);

        model::Year retrieveRecord(model::Year&, type::YearFilter);

        bool doesYearExist(const model::Year&, type::YearFilter);

        void saveRecord(const model::Year&);
        void deleteYear(const model::Year&, type::YearFilter);
    private:
        std::vector<model::Year> parseRecords(MYSQL_STMT*);

        std::pair<model::Year, int> parseRecordWithSongCount(MYSQL_STMT*);

        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        model::Year parseRecord(MYSQL_RES*);
        model::Year parseRecord(MYSQL_STMT*);
    };
}

#endif
