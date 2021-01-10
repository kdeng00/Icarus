#ifndef YEARREPOSITORY_H_
#define YEARREPOSITORY_H_

#include <memory>
#include <utility>
#include <vector>

#include "icarus_lib/icarus.h"

#include "database/BaseRepository.h"
#include "type/YearFilter.h"

namespace database {
    class YearRepository : public BaseRepository {
    public:
        YearRepository(const icarus_lib::binary_path &);

        std::vector<icarus_lib::year> retrieveRecords();

        std::pair<icarus_lib::year, int> retrieveRecordWithSongCount(icarus_lib::year&, 
                type::YearFilter = type::YearFilter::id);

        icarus_lib::year retrieveRecord(icarus_lib::year&, type::YearFilter = type::YearFilter::id);

        bool doesYearExist(const icarus_lib::year&, type::YearFilter = type::YearFilter::id);

        void saveRecord(const icarus_lib::year&);
        void deleteYear(const icarus_lib::year&, type::YearFilter = type::YearFilter::id);
    private:
        std::vector<icarus_lib::year> parseRecords(MYSQL_STMT*);

        std::pair<icarus_lib::year, int> parseRecordWithSongCount(MYSQL_STMT*);

        std::shared_ptr<MYSQL_BIND> valueBind(icarus_lib::year&);
        std::shared_ptr<MYSQL_BIND> valueBindWithSongCount(icarus_lib::year&,
                int&);

        icarus_lib::year parseRecord(MYSQL_STMT*);
    };
}

#endif
