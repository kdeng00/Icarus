using System;
using System.Collections.Generic;
using System.IO;

using Icarus.Controllers.Utilities;
using Icarus.Database.Repositories;
using Icarus.Models;

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
            // TODO: Implement saving Cover Art record to
            // the database.
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

                Console.WriteLine($"Cover path {imagePath}");
                var metaData = new MetadataRetriever();
                var imgBytes = metaData.RetrieveCoverArtBytes(song);
                
                Console.WriteLine("Saving image");
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
