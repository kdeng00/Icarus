#include "managers/coverArtManager.h"

#include "database/coverArtRepository.h"
#include "types/coverFilter.h"
#include "utilities/metadata_retriever.h"

Manager::coverArtManager::coverArtManager(const std::string& configPath) : path(configPath)
{ }


Model::Cover Manager::coverArtManager::saveCover(const Model::Song& song, std::string& rootPath, const std::string& stockCoverPath)
{
    metadata_retriever meta;
    Model::Cover cov;
    cov.imagePath = rootPath;
    cov = meta.update_cover_art(song, cov, stockCoverPath);
    cov.songTitle = song.title;

    coverArtRepository covRepo(path);
    std::cout << "saving record to the database" << std::endl;
    covRepo.saveRecord(cov);
    std::cout << "retrieving record from database" << std::endl;
    cov = covRepo.retrieveRecord(cov, coverFilter::songTitle);

    return cov;
}
