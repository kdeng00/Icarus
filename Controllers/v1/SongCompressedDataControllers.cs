using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/song/compressed/data")]
    [ApiController]
    public class SongCompressedDataController : ControllerBase
    {
        #region Fields
        private string _connectionString;
        private IConfiguration _config;
        private string _songTempDir;
        private string _archiveDir;
        #endregion


        #region Properties
        #endregion


        #region Constructor
        public SongCompressedDataController(IConfiguration config)
        {
            _songTempDir = _config.GetValue<string>("TemporaryMusicPath");
            _archiveDir = _config.GetValue<string>("ArchivePath");
            _config = config;
            _connectionString = _config.GetConnectionString("DefaultConnection");
        }
        #endregion


        #region API Routes
        [HttpGet("{id}")]
        [Authorize("download:songs")]
        public async Task<IActionResult> Get(int id)
        {
            var context = new SongContext(_connectionString);

            SongCompression cmp = new SongCompression(_archiveDir);
        
            Console.WriteLine($"Archive directory root: {_archiveDir}");

            Console.WriteLine("Starting process of retrieving comrpessed song");
            SongData song = await cmp.RetrieveCompressedSong(context.RetrieveRecord(new Song{ SongID = id }));

            return File(song.Data, "application/x-msdownload", cmp.CompressedSongFilename);
        }
        #endregion
    }
}
