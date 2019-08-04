using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

using Icarus.Constants;
using Icarus.Controllers.Utilities;
using Icarus.Database.Repositories;
using Icarus.Models;
using Icarus.Types;

namespace Icarus.Controllers.Managers
{
    public class CoverArtManager : BaseManager
    {
        #region Fields
        private string _rootCoverArtPath;
        #endregion


        #region Constructors
        public CoverArtManager(string rootPath)
        {
            _rootCoverArtPath = rootPath;
            Initialize();
        }
        #endregion


        #region Methods
        public void SaveCoverArtToDatabase(ref Song song, ref CoverArt coverArt, 
                CoverArtRepository coverArtRepository)
        {
            _logger.Info("Saving cover art record to the database");
            coverArtRepository.SaveCoverArt(coverArt);

            coverArt = coverArtRepository.GetCoverArt(CoverArtField.SongTitle,
                    coverArt);

            song.CoverArtId = coverArt.CoverArtId;
            _logger.Info("Cover art Process saving complete");
        }
        public void DeleteCoverArtFromDatabase(CoverArt coverArt, 
                CoverArtRepository coverArtRepository)
        {
            _logger.Info("Attempting to delete cover art from the database");
            coverArtRepository.DeleteCoverArt(coverArt);
        }
        public void DeleteCoverArt(CoverArt coverArt)
        {
            var stockCoverArtPath = _rootCoverArtPath + "CoverArtPath.png";
            DirectoryManager.delete_cover_art(coverArt.ImagePath, stockCoverArtPath);
        }

        public CoverArt SaveCoverArt(Song song)
        {
            try
            {
                // TODO: Change logic so it attempts to create the directory
                // after it has been determined that the song does not have
                // a cover art image
                
                var strCount = _rootCoverArtPath.Length + song.Artist.Length + 
                    song.AlbumTitle.Length + 2;
                var imgPath = new StringBuilder(strCount);

                DirectoryManager.create_directory(SongManager.ConvertSongToSng(song), 
                        _rootCoverArtPath, imgPath);

                var imagePath = imgPath.ToString().Substring(0, strCount);
                imagePath += song.Title + ".png";
                var coverArt = new CoverArt
                {
                    SongTitle = song.Title,
                    ImagePath = imagePath
                };

                var metaData = new MetadataRetriever();
                var imgBytes = metaData.RetrieveCoverArtBytes(song);
                
                if (imgBytes != null)
                {
                    _logger.Info("Saving cover art to the filesystem");
                    File.WriteAllBytes(coverArt.ImagePath, imgBytes);
                }
                else
                {
                    _logger.Info("Song has no cover art, applying stock cover art");
                    coverArt.ImagePath = _rootCoverArtPath + "CoverArt.png";
                    metaData.UpdateCoverArt(song, coverArt);
                }

                return coverArt;
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
            
            return null;
        }

        private void Initialize()
        {
            var stockCover = _rootCoverArtPath + "CoverArt.png";
            DirectoryManager.copy_stock_cover_art(stockCover, 
                    DirectoryPaths.CoverArtPath);
        }
        #region Testing
        private void PrintCoverArtDetails(CoverArt cover)
        {
            Console.WriteLine("\nCover art");
            Console.WriteLine($"Song Title: {cover.SongTitle}");
            Console.WriteLine($"ImagePath: {cover.ImagePath}\n");
        }
        #endregion
        #endregion
    }
}
