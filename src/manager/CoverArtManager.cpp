#include "manager/CoverArtManager.h"

#include <iostream>
#include <filesystem>

#include "database/CoverArtRepository.h"
#include "manager/DirectoryManager.h"
#include "type/CoverFilter.h"
#include "utility/MetadataRetriever.h"

namespace fs = std::filesystem;

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


void manager::CoverArtManager::deleteCover(const model::Song& song)
{
    database::CoverArtRepository covRepo(m_bConf);

    model::Cover cov(song.coverArtId);

    cov = covRepo.retrieveRecord(cov, type::CoverFilter::id);
    covRepo.deleteRecord(cov);

    auto paths = manager::DirectoryManager::pathConfigContent(m_bConf);

    const auto coverArtPath = paths["cover_root_path"].get<std::string>();
    std::string stockCoverArtPath = coverArtPath;
    stockCoverArtPath.append("CoverArt.png");

    if (stockCoverArtPath.compare(cov.imagePath) != 0) {
        fs::remove(cov.imagePath);
        std::cout << "deleting cover art" << std::endl;
    } else {
        std::cout << "song contains the stock cover art, will not delete" << std::endl;
    }

    manager::DirectoryManager::deleteDirectories(song, coverArtPath);
}
