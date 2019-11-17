#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>
#include <string.h>

#include <attachedpictureframe.h>
#include <textidentificationframe.h>
#include <fileref.h>
#include <mpegfile.h>
#include <tag.h>

#include "manager/DirectoryManager.h"
#include "utility/ImageFile.h"
#include "utility/MetadataRetriever.h"

namespace fs = std::filesystem;

namespace utility {
    model::Song MetadataRetriever::retrieveMetadata(model::Song& song) {
		TagLib::MPEG::File sameFile(song.songPath.c_str());
		auto tag = sameFile.ID3v2Tag();
		song.title = tag->title().toCString();
		song.artist = tag->artist().toCString();
		song.album = tag->album().toCString();
		song.genre = tag->genre().toCString();
		song.year = tag->year();
		song.track = tag->track();
		song.duration = sameFile.audioProperties()->lengthInSeconds();

		constexpr auto id3DiscName = "TPOS";
		constexpr auto id3AlbumArtistName = "TPE2";

		auto discFrame = tag->frameList(id3DiscName);
		auto albumArtistFrame = tag->frameList(id3AlbumArtistName);
		if (discFrame.isEmpty()) {
		    constexpr auto discDefaultVal = "1";
		    TagLib::ID3v2::TextIdentificationFrame *emptyFrame = 
		            new TagLib::ID3v2::TextIdentificationFrame(id3DiscName);
		    tag->addFrame(emptyFrame);
		    emptyFrame->setText(discDefaultVal);
		    song.disc = std::atoi(discDefaultVal);
		    sameFile.save();
		} else {
		    song.disc = std::stoi(discFrame.front()->toString().toCString());
		}

		if (albumArtistFrame.isEmpty()) {
		    TagLib::ID3v2::TextIdentificationFrame *emptyFrame = 
		            new TagLib::ID3v2::TextIdentificationFrame(id3DiscName);
		    tag->addFrame(emptyFrame);
		    emptyFrame->setText(song.artist.c_str());
		    sameFile.save();
		} else {
		    song.albumArtist = albumArtistFrame.front()->toString().toCString();
		}

		return song;
    }

    model::Cover MetadataRetriever::updateCoverArt(const model::Song& song, model::Cover& cov, 
		    const std::string& stockCoverPath) {
		TagLib::MPEG::File sngF(song.songPath.c_str());
		auto tag = sngF.ID3v2Tag();
		auto frameList = tag->frameListMap()["APIC"];

		if (frameList.isEmpty()) {
		    cov.imagePath.append("CoverArt.png");
        
		    if (!fs::exists(cov.imagePath)) {
		        std::cout << "copying stock cover path\n";
		        fs::copy(stockCoverPath, cov.imagePath);
		    }

		    ImageFile stockImg(cov.imagePath.c_str());

		    TagLib::ID3v2::AttachedPictureFrame *pic = 
				    new TagLib::ID3v2::AttachedPictureFrame;
		    pic->setPicture(stockImg.data());
		    pic->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);

		    tag->addFrame(pic);

		    sngF.save();
		    std::cout << "applied stock cover art" << std::endl;
		} else {
		    auto frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(
		            frameList.front());
        
		    auto imgPath = manager::DirectoryManager::createDirectoryProcess(song, cov.imagePath);
		    imgPath.append(song.title);
		    imgPath.append(".png");
		    cov.imagePath = imgPath;

		    std::fstream imgSave(cov.imagePath, std::ios::out | 
		            std::ios::binary);
		    imgSave.write(frame->picture().data(), frame->picture().size());
		    imgSave.close();
		    std::cout << "saved to " << cov.imagePath << "\n";
		}

		return cov;
    }

    // tags song with the stock cover art
    model::Cover MetadataRetriever::applyStockCoverArt(
		    const model::Song& song, model::Cover& cov, 
		    const std::string& stockCoverPath) {
		TagLib::MPEG::File songFile(song.songPath.c_str());
		auto tag = songFile.ID3v2Tag();

		ImageFile stockImg(cov.imagePath.c_str());
		TagLib::ID3v2::AttachedPictureFrame *pic = 
				new TagLib::ID3v2::AttachedPictureFrame;

		pic->setPicture(stockImg.data());
		pic->setType(TagLib::ID3v2::AttachedPictureFrame::FrontCover);

		tag->addFrame(pic);

		songFile.save();
		std::cout << "applied stock cover art\n";

		delete pic;

		return cov;
    }

    // extracts cover art from the song and save it to the
    // appropriate directory
    model::Cover MetadataRetriever::applyCoverArt(const model::Song& song,
		    model::Cover& cov) {
		TagLib::MPEG::File songFile(song.songPath.c_str());
		auto tag = songFile.ID3v2Tag();
		auto frameList = tag->frameListMap()["APIC"];

		auto frame = dynamic_cast<TagLib::ID3v2::AttachedPictureFrame*>(
				frameList.front());
        
		std::fstream imgSave(cov.imagePath, std::ios::out | 
				std::ios::binary);
		imgSave.write(frame->picture().data(), frame->picture().size());
		imgSave.close();

		std::cout << "saved to " << cov.imagePath << "\n";

		return cov;
    }


    bool MetadataRetriever::songContainsCoverArt(const model::Song& song) {
		TagLib::MPEG::File songFile(song.songPath.c_str());
		auto tag = songFile.ID3v2Tag();
		auto frameList = tag->frameListMap()["APIC"];

		return !frameList.isEmpty();
    }


    void MetadataRetriever::updateMetadata(model::Song& sngUpdated, const model::Song& sngOld) {
		std::cout << "updating metadata\n";
		TagLib::MPEG::File file(sngOld.songPath.c_str());
		auto tag = file.ID3v2Tag();
    
		if (sngUpdated.title.size() > 0) {
		    file.tag()->setTitle(sngUpdated.title);
		}
		if (sngUpdated.artist.size() > 0) {
		    file.tag()->setArtist(sngUpdated.artist);
		}
		if (sngUpdated.albumArtist.size() > 0) {
		    constexpr auto frameId = "TPE2";
		    auto albumArtistFrame = tag->frameList(frameId);
		    if (albumArtistFrame.isEmpty()) {
		        TagLib::ID3v2::TextIdentificationFrame *frame =
		                new TagLib::ID3v2::TextIdentificationFrame(frameId);
		        frame->setText(sngUpdated.albumArtist.c_str());
		        tag->addFrame(frame);
		    } else {
		        albumArtistFrame.front()->setText(sngUpdated.albumArtist.c_str());
		    }

		    file.save();
		}
		if (sngUpdated.album.size() > 0) {
		    file.tag()->setAlbum(sngUpdated.album);
		}
		if (sngUpdated.genre.size() > 0) {
		    file.tag()->setGenre(sngUpdated.genre);
		}
		if (sngUpdated.year > 0) {
		    file.tag()->setYear(sngUpdated.year);
		}

        // TODO: functionality to update the track number and disc number

		file.save();
    }
}
