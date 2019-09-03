#ifndef ALBUMREPOSITORY_H_
#define ALBUMREPOSITORY_H_

#include <vector>

#include "database/base_repository.h"
#include "models/models.h"
#include "types/albumFilter.h"

namespace Database
{
    class albumRepository : public base_repository
    {
    public:
        albumRepository(const Model::BinaryPath&);

        std::vector<Model::Album> retrieveRecords();

        Model::Album retrieveRecord(Model::Album&, Type::albumFilter);

        bool doesAlbumExists(const Model::Album&, Type::albumFilter);

        void saveAlbum(const Model::Album&);
    private:
        std::vector<Model::Album> parseRecords(MYSQL_RES*);

        // TODO: after parseRecord(MYSQL_STMT*) is implemented remove
        // parseRecord(MYSQL_RES*)
        Model::Album parseRecord(MYSQL_RES*);
        Model::Album parseRecord(MYSQL_STMT*);
    };
}

#endif
