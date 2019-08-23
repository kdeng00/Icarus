#include "managers/coverArtManager.h"

#include "utilities/metadata_retriever.h"

Cover coverArtManager::saveCover(const Song& song, std::string& rootPath, const std::string& stockCoverPath)
{
    metadata_retriever meta;
    Cover cov;
    cov.imagePath = rootPath;
    cov = meta.update_cover_art(song, cov, stockCoverPath);
    cov.songTitle = song.title;

    return cov;
}
