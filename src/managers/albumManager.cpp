#include "managers/albumManager.h"

#include "database/albumRepository.h"
#include "models/models.h"

Manager::albumManager::albumManager(const Model::BinaryPath& bConf)
    : m_bConf(bConf)
{ }


void Manager::albumManager::saveAlbum(const Model::Song& song)
{
    Model::Album album;
    album.title = song.album;
    album.year = song.year;
    
    Database::albumRepository albRepo(m_bConf);
    albRepo.saveAlbum(album);
}
