#ifndef YEARREPOSITORY_H_
#define YEARREPOSITORY_H_

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

        model::Year retrieveRecord(model::Year&, type::YearFilter);

        bool doesYearExist(const model::Year&, type::YearFilter);

        void saveRecord(const model::Year&);
    private:
        std::vector<model::Year> parseRecords(MYSQL_STMT*);

        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        model::Year parseRecord(MYSQL_RES*);
        model::Year parseRecord(MYSQL_STMT*);
    };
}

#endif
