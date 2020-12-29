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
    CoverArtManager::CoverArtManager(const icarus_lib::binary_path & bConf) : m_bConf(bConf) { }


    icarus_lib::cover CoverArtManager::saveCover(const icarus_lib::song& song) {
        auto pathConfigContent = DirectoryManager::pathConfigContent(m_bConf);
        auto stockCoverPath = DirectoryManager::configPath(m_bConf);
        stockCoverPath.append("/CoverArt.png");

        utility::MetadataRetriever meta;
        icarus_lib::cover cov;
        cov.song_title = song.title;

        if (meta.songContainsCoverArt(song)) {
            cov.image_path = createImagePath(song);
            cov = meta.applyCoverArt(song, cov);
        } else {
            cov.image_path = pathConfigContent["cover_root_path"].get<std::string>();
            cov = meta.applyStockCoverArt(song, cov, stockCoverPath);
        }

        database::CoverArtRepository covRepo(m_bConf);
        if (!covRepo.doesCoverArtExist(cov, type::CoverFilter::songTitle)) {
            std::cout << "saving image record to the database\n";
            covRepo.saveRecord(cov);
        } else {
            std::cout << "cover art record already exists\n";
        }

        std::cout << "retrieving image record from database\n";
        cov = covRepo.retrieveRecord(cov, type::CoverFilter::songTitle);

        return cov;
    }


    std::pair<bool, std::string> CoverArtManager::defaultCover(
            const icarus_lib::cover& cover) {

        auto paths = DirectoryManager::pathConfigContent(m_bConf);
        const auto coverArtPath = 
            paths[DirectoryManager::retrievePathType(
                type::PathType::coverArt)].get<std::string>();

        auto stockCoverArtPath = coverArtPath;
        stockCoverArtPath.append("CoverArt.png");

        if (stockCoverArtPath.compare(cover.image_path) == 0) {
            return std::make_pair(true, coverArtPath);
        } else {
            return std::make_pair(false, coverArtPath);
        }
    }


    void CoverArtManager::deleteCover(const icarus_lib::song& song) {
        database::CoverArtRepository covRepo(m_bConf);

        icarus_lib::cover cov(song.cover_art_id);

        cov = covRepo.retrieveRecord(cov, type::CoverFilter::id);
        covRepo.deleteRecord(cov);

        auto result = defaultCover(cov);
        if (!result.first) {
            fs::remove(cov.image_path);
            std::cout << "deleting cover art\n";
            const auto coverArtPath = result.second;
        } else {
            std::cout << "song contains the stock cover art, will not delete\n";
            return;
        }

        DirectoryManager::deleteDirectories(song, result.second);
    }

    void CoverArtManager::updateCover(const icarus_lib::song& updatedSong,
            const icarus_lib::song& currSong) {
        database::CoverArtRepository covRepo(m_bConf);
        icarus_lib::cover cover(updatedSong.cover_art_id);
        cover = covRepo.retrieveRecord(cover, type::CoverFilter::id);

        auto result = defaultCover(cover);

        if (result.first) {
            return;
        }

        auto image_path = createImagePath(updatedSong);

        fs::copy(cover.image_path, image_path);
        fs::remove(cover.image_path);

        DirectoryManager::deleteDirectories(currSong, result.second);
    }

    void CoverArtManager::updateCoverRecord(const icarus_lib::song& updatedSong) {
        icarus_lib::cover updatedCover(updatedSong);
        auto updatedImagePath = createImagePath(updatedSong);

        database::CoverArtRepository covRepo(m_bConf);
        covRepo.updateRecord(updatedCover);
    }


    std::string CoverArtManager::createImagePath(const icarus_lib::song& song) {
        auto image_path = DirectoryManager::createDirectoryProcess(
                song, m_bConf, type::PathType::coverArt);

        if (song.track != 0) {
            image_path.append("track");
            auto trackNum = (song.track > 9) ?
                    std::to_string(song.track) : "0" + std::to_string(song.track);
            image_path.append(trackNum);
        } else {
            image_path.append(song.title);
        }
        image_path.append(".png");

        return image_path;
    }
}
