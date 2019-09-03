#include "managers/coverArtManager.h"

#include "database/coverArtRepository.h"
#include "types/coverFilter.h"
#include "utilities/metadata_retriever.h"

Manager::coverArtManager::coverArtManager(const std::string& configPath) : path(configPath)
{ }

Manager::coverArtManager::coverArtManager(const Model::BinaryPath& bConf) : m_bConf(bConf)
{ }


Model::Cover Manager::coverArtManager::saveCover(const Model::Song& song, std::string& rootPath, const std::string& stockCoverPath)
{
    Utility::metadata_retriever meta;
    Model::Cover cov;
    cov.imagePath = rootPath;
    cov = meta.update_cover_art(song, cov, stockCoverPath);
    cov.songTitle = song.title;

    Database::coverArtRepository covRepo(m_bConf);
    if (!covRepo.doesCoverArtExist(cov, Type::coverFilter::songTitle)) {
        std::cout << "saving image record to the database" << std::endl;
        covRepo.saveRecord(cov);
    } else {
        std::cout << "cover art record already exists" << std::endl;
    }

    std::cout << "retrieving image record from database" << std::endl;
    cov = covRepo.retrieveRecord(cov, Type::coverFilter::songTitle);

    return cov;
}
