#include "manager/CoverArtManager.h"

#include "database/CoverArtRepository.h"
#include "type/CoverFilter.h"
#include "utility/MetadataRetriever.h"

manager::CoverArtManager::CoverArtManager(const std::string& configPath) : path(configPath)
{ }

manager::CoverArtManager::CoverArtManager(const model::BinaryPath& bConf) : m_bConf(bConf)
{ }


model::Cover manager::CoverArtManager::saveCover(const model::Song& song, std::string& rootPath, const std::string& stockCoverPath)
{
    utility::MetadataRetriever meta;
    model::Cover cov;
    cov.imagePath = rootPath;
    cov = meta.updateCoverArt(song, cov, stockCoverPath);
    cov.songTitle = song.title;

    database::CoverArtRepository covRepo(m_bConf);
    if (!covRepo.doesCoverArtExist(cov, type::CoverFilter::songTitle)) {
        std::cout << "saving image record to the database" << std::endl;
        covRepo.saveRecord(cov);
    } else {
        std::cout << "cover art record already exists" << std::endl;
    }

    std::cout << "retrieving image record from database" << std::endl;
    cov = covRepo.retrieveRecord(cov, type::CoverFilter::songTitle);

    return cov;
}
