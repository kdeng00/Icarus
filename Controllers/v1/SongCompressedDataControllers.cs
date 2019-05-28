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
using Icarus.Database.Repositories;

namespace Icarus.Controllers.V1
{
	[Route("api/v1/song/compressed/data")]
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
		}
		#endregion


		#region API Routes
        	[HttpGet("{id}")]
		[Authorize("download:songs")]
        	public async Task<IActionResult> Get(int id)
        	{
	   		 SongRepository context = HttpContext
				 .RequestServices
				 .GetService(typeof(SongRepository)) as SongRepository;

  	    		SongCompression cmp = new SongCompression(_archiveDir);
	    
            		Console.WriteLine($"Archive directory root: {_archiveDir}");

	    		Console.WriteLine("Starting process of retrieving comrpessed song");
	    		SongData song = await cmp.RetrieveCompressedSong(context.GetSong(id));

	    		return File(song.Data, "application/x-msdownload", cmp.CompressedSongFilename);
        	}
        	#endregion
    	}
}
