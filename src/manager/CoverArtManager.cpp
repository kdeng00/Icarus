#include "manager/CoverArtManager.h"

#include <iostream>
#include <filesystem>

#include "database/CoverArtRepository.h"
#include "manager/DirectoryManager.h"
#include "type/CoverFilter.h"
#include "type/PathType.h"
#include "utility/MetadataRetriever.h"

namespace fs = std::filesystem;

namespace manager {
CoverArtManager::CoverArtManager(const model::BinaryPath& bConf) : m_bConf(bConf) { }


model::Cover CoverArtManager::saveCover(const model::Song& song)
{
    auto pathConfigContent = DirectoryManager::pathConfigContent(m_bConf);
    auto stockCoverPath = DirectoryManager::configPath(m_bConf);
    stockCoverPath.append("/CoverArt.png");

    utility::MetadataRetriever meta;
    model::Cover cov;
    cov.songTitle = song.title;

    if (meta.songContainsCoverArt(song)) {
        cov.imagePath = createImagePath(song);
        cov = meta.applyCoverArt(song, cov);
    } else {
        cov.imagePath = pathConfigContent["cover_root_path"].get<std::string>();
        cov = meta.applyStockCoverArt(song, cov, stockCoverPath);
    }

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


std::pair<bool, std::string> CoverArtManager::defaultCover(
    const model::Cover& cover) {

    auto paths = DirectoryManager::pathConfigContent(m_bConf);
    const auto coverArtPath = 
        paths[DirectoryManager::retrievePathType(
            type::PathType::coverArt)].get<std::string>();

    auto stockCoverArtPath = coverArtPath;
    stockCoverArtPath.append("CoverArt.png");

    if (stockCoverArtPath.compare(cover.imagePath) == 0) {
        return std::make_pair(true, coverArtPath);
    } else {
        return std::make_pair(false, coverArtPath);
    }
}


void CoverArtManager::deleteCover(const model::Song& song)
{
    database::CoverArtRepository covRepo(m_bConf);

    model::Cover cov(song.coverArtId);

    cov = covRepo.retrieveRecord(cov, type::CoverFilter::id);
    covRepo.deleteRecord(cov);

    auto result = defaultCover(cov);
    if (!result.first) {
        fs::remove(cov.imagePath);
        std::cout << "deleting cover art" << std::endl;
        const auto coverArtPath = result.second;
    } else {
        std::cout << "song contains the stock cover art, will not delete" << std::endl;
        return;
    }

    DirectoryManager::deleteDirectories(song, result.second);
}

void CoverArtManager::updateCover(const model::Song& updatedSong,
        const model::Song& currSong)
{
    database::CoverArtRepository covRepo(m_bConf);
    model::Cover cover(updatedSong.coverArtId);
    cover = covRepo.retrieveRecord(cover, type::CoverFilter::id);

    auto result = defaultCover(cover);

    if (result.first) {
        return;
    }

    auto imagePath = createImagePath(updatedSong);

    fs::copy(cover.imagePath, imagePath);
    fs::remove(cover.imagePath);

    DirectoryManager::deleteDirectories(currSong, result.second);
}

void CoverArtManager::updateCoverRecord(const model::Song& updatedSong)
{
    model::Cover updatedCover(updatedSong);
    auto updatedImagePath = createImagePath(updatedSong);

    database::CoverArtRepository covRepo(m_bConf);
    covRepo.updateRecord(updatedCover);
}


std::string CoverArtManager::createImagePath(const model::Song& song)
{
    auto imagePath = DirectoryManager::createDirectoryProcess(
        song, m_bConf, type::PathType::coverArt);

    if (song.track != 0) {
        imagePath.append("track");
        auto trackNum = (song.track > 9) ?
            std::to_string(song.track) : "0" + std::to_string(song.track);
        imagePath.append(trackNum);
    } else {
        imagePath.append(song.title);
    }
    imagePath.append(".png");

    return imagePath;
}
}
