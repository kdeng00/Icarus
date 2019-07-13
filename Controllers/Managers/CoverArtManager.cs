using System;
using System.Collections.Generic;
using System.IO;

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
        }
        #endregion


        #region Methods
        public void SaveCoverArtToDatabase(ref Song song, ref CoverArt coverArt, 
                CoverArtRepository coverArtRepository)
        {
            coverArtRepository.SaveCoverArt(coverArt);

            coverArt = coverArtRepository.GetCoverArt(CoverArtField.SongTitle,
                    coverArt);

            song.CoverArtId = coverArt.CoverArtId;
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
                
                File.WriteAllBytes(imagePath, imgBytes);

                return coverArt;
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
                Console.WriteLine(msg);
            }
            
            return null;
        }
        #endregion
    }
}
