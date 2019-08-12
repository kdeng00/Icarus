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
            var sng = SongManager.ConvertSongToSng(song);

            var cov = ConvertCoverArtToCovArt(new CoverArt
            {
                SongTitle = song.Title,
                ImagePath = _rootCoverArtPath
            });

            MetadataRetriever.update_cover_art(ref cov, 
                    ref sng, _rootCoverArtPath);

            return ConvertCovArtToCoverArt(cov);
        }

        public static CovArt ConvertCoverArtToCovArt(CoverArt cover)
        {
            return new CovArt
            {
                SongTitle = cover.SongTitle,
                ImagePath = cover.ImagePath
            };
        }
        public static CoverArt ConvertCovArtToCoverArt(CovArt cov)
        {
            return new CoverArt
            {
                SongTitle = cov.SongTitle,
                ImagePath = cov.ImagePath
            };
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
