#include "manager/CoverArtManager.h"

#include <iostream>
#include <filesystem>

#include "database/CoverArtRepository.h"
#include "manager/DirectoryManager.h"
#include "type/CoverFilter.h"
#include "type/PathType.h"
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


std::pair<bool, std::string> manager::CoverArtManager::defaultCover(
    const model::Cover& cover) {

    auto paths = manager::DirectoryManager::pathConfigContent(m_bConf);
    const auto coverArtPath = 
        paths[manager::DirectoryManager::retrievePathType(
            type::PathType::coverArt)].get<std::string>();

    auto stockCoverArtPath = coverArtPath;
    stockCoverArtPath.append("CoverArt.png");

    if (stockCoverArtPath.compare(cover.imagePath) == 0) {
        return std::make_pair(true, coverArtPath);
    } else {
        return std::make_pair(false, coverArtPath);
    }
}


void manager::CoverArtManager::deleteCover(const model::Song& song)
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
    }

    manager::DirectoryManager::deleteDirectories(song, result.second);
}

void manager::CoverArtManager::updateCover(const model::Song& updatedSong)
{
    database::CoverArtRepository covRepo(m_bConf);
    model::Cover cover(updatedSong.coverArtId);
    cover = covRepo.retrieveRecord(cover, type::CoverFilter::id);

    auto result = defaultCover(cover);

    if (result.first) {
        return;
    }

    auto imagePath = manager::DirectoryManager::createDirectoryProcess(
        updatedSong, m_bConf, type::PathType::coverArt);
    imagePath.append(updatedSong.title);
    imagePath.append(".png");

    if (cover.imagePath.find(".png") == std::string::npos) {
        std::cout << "no file extension on image path" << std::endl;
        cover.imagePath.append(".png");
    }

    fs::copy(cover.imagePath, imagePath);
    fs::remove(cover.imagePath);
}

void manager::CoverArtManager::updateCoverRecord(const model::Song& updatedSong)
{
    model::Cover updatedCover(updatedSong);
    auto updatedImagePath = manager::DirectoryManager::createDirectoryProcess(
        updatedSong, m_bConf, type::PathType::coverArt);
    updatedImagePath.append(updatedSong.title);

    updatedCover.imagePath = std::move(updatedImagePath);
    updatedCover.imagePath.append(".png");

    database::CoverArtRepository covRepo(m_bConf);
    covRepo.updateRecord(updatedCover);
}
