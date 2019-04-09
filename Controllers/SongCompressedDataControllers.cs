using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Models;

namespace Icarus.Controllers
{
    [Route("api/song/compressed/data")]
    [ApiController]
    public class SongCompressedDataController : ControllerBase
    {
        #region Fields
        private IConfiguration _config;
		private SongManager _songMgr;
		private string _songTempDir;
		private string _archiveDir;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongCompressedDataController(IConfiguration config)
		{
			_config = config;
			_songTempDir = _config.GetValue<string>("TemporaryMusicPath");
			_archiveDir = _config.GetValue<string>("ArchivePath");
			_songMgr = new SongManager(config, _songTempDir);
			_songMgr.ArchiveDirectoryRoot = _archiveDir;
		}
		#endregion


		#region API Routes
        [HttpGet("{id}")]
        public async Task<IActionResult> Get(int id)
        {
			SongData song = new SongData();

			Console.WriteLine($"Archive directory root: {_archiveDir}");

			Console.WriteLine("Starting process of retrieving comrpessed song");
			song = await _songMgr.RetrieveCompressedSong(id);

			return File(song.Data, "application/x-msdownload", _songMgr.CompressedSongFilename);
        }
		#endregion
    }
}
