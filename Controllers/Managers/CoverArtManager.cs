using System;
using System.Collections.Generic;

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
        public bool SaveCoverArt(Song song)
        {
            // TODO: Implement functionality to save
            // cover art to the filesystem. The result
            // will determine if the the cover art record will
            // be saved to the DB
            
            return false;
        }
        #endregion
    }
}
