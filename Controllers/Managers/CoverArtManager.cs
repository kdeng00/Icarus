using System;
using System.Collections.Generic;
using System.IO;

using Icarus.Constants;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Types;

namespace Icarus.Controllers.Managers
{
    public class CoverArtManager : BaseManager
    {
        #region Fields
        private string _rootCoverArtPath;
        private byte[] _stockCoverArt = null;
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
        }
        public void DeleteCoverArtFromDatabase(CoverArt coverArt, 
                CoverArtRepository coverArtRepository)
        {
            _logger.Info("Attempting to delete cover art from the database");
            coverArtRepository.DeleteCoverArt(coverArt);
        }
        public void DeleteCoverArt(CoverArt coverArt)
        {
            try
            {
                var stockCoverArtPath = _rootCoverArtPath + "CoverArt.png";
                if (!string.Equals(stockCoverArtPath, coverArt.ImagePath, 
                            StringComparison.CurrentCultureIgnoreCase))
                {
                    _logger.Info("Song does not contain the stock cover art");
                    File.Delete(coverArt.ImagePath);
                    _logger.Info("Cover art deleted from the filesystem");
                }
                else
                {
                    _logger.Info("Song contains the stock cover art");
                    _logger.Info("Will not delete from from the filesystem");
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
        }

        public CoverArt SaveCoverArt(Song song)
        {
            try
            {
                var dirMgr = new DirectoryManager(_rootCoverArtPath);
                dirMgr.CreateDirectory(song);
                var imagePath = dirMgr.SongDirectory + song.Title + ".png";
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
                    File.WriteAllBytes(coverArt.ImagePath, _stockCoverArt);
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
            if (System.IO.File.Exists(DirectoryPaths.CoverArtPath))
                _stockCoverArt = File.ReadAllBytes(DirectoryPaths.CoverArtPath);

            if (!File.Exists(_rootCoverArtPath + "CoverArt.png"))
            {
                File.WriteAllBytes(_rootCoverArtPath + "CoverArt.png", 
                        _stockCoverArt);
                Console.WriteLine("Copied Stock Cover Art");
            }
        }
        #endregion
    }
}
