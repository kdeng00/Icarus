#ifndef YEARREPOSITORY_H_
#define YEARREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/YearFilter.h"

namespace database {
    class YearRepository : public BaseRepository {
    public:
        YearRepository(const model::BinaryPath&);

        std::vector<model::Year> retrieveRecords();

        std::pair<model::Year, int> retrieveRecordWithSongCount(model::Year&, 
                type::YearFilter = type::YearFilter::id);

        model::Year retrieveRecord(model::Year&, type::YearFilter = type::YearFilter::id);

        bool doesYearExist(const model::Year&, type::YearFilter = type::YearFilter::id);

        void saveRecord(const model::Year&);
        void deleteYear(const model::Year&, type::YearFilter = type::YearFilter::id);
    private:
        std::vector<model::Year> parseRecords(MYSQL_STMT*);

        std::pair<model::Year, int> parseRecordWithSongCount(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(model::Year&);
        std::shared_ptr<MYSQL_BIND> valueBindWithSongCount(model::Year&,
                int&);

        model::Year parseRecord(MYSQL_STMT*);
    };
}

#endif
