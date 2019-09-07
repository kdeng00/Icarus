#ifndef ALBUMREPOSITORY_H_
#define ALBUMREPOSITORY_H_

#include <vector>

#include "database/BaseRepository.h"
#include "model/Models.h"
#include "type/AlbumFilter.h"

namespace database
{
    class AlbumRepository : public BaseRepository
    {
    public:
        AlbumRepository(const model::BinaryPath&);

        std::vector<model::Album> retrieveRecords();

        model::Album retrieveRecord(model::Album&, type::AlbumFilter);

        bool doesAlbumExists(const model::Album&, type::AlbumFilter);

        void saveAlbum(const model::Album&);
    private:
        std::vector<model::Album> parseRecords(MYSQL_STMT*);

        // TODO: after parseRecord(MYSQL_STMT*) is implemented remove
        // parseRecord(MYSQL_RES*)
        model::Album parseRecord(MYSQL_RES*);
        model::Album parseRecord(MYSQL_STMT*);
    };
}

#endif
