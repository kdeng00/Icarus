#ifndef YEARREPOSITORY_H_
#define YEARREPOSITORY_H_

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/YearFilter.h"

namespace database
{
    class YearRepository : public BaseRepository
    {
    public:
        YearRepository(const model::BinaryPath&);

        model::Year retrieveRecord(model::Year&, type::YearFilter);

        bool doesYearExist(const model::Year&, type::YearFilter);

        void saveRecord(const model::Year&);
    private:
        // TODO: After parseRecord(MYSQL_STMT*) is implemented
        // remove parseRecord(MYSQL_RES*)
        model::Year parseRecord(MYSQL_RES*);
        model::Year parseRecord(MYSQL_STMT*);
    };
}

#endif
