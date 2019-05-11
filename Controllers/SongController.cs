using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
	[Route("api/song")]
	[ApiController]
	// TODO: Secure the HTTP endpoint routes with Auth0 grants. #39
    	public class SongController : ControllerBase
    	{
		#region Fields
		private readonly ILogger<SongController> _logger;
		private IConfiguration _config;
		private MusicStoreContext _context;
		private SongManager _songMgr;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongController(IConfiguration config, ILogger<SongController> logger)
		{
			_config = config;
			_logger = logger;
			_songMgr = new SongManager(config);
		}
		#endregion


        	[HttpGet]
		[Authorize("read:song_details")]
        	public IActionResult Get()
        	{
			List<Song> songs = new List<Song>();
			Console.WriteLine("Attemtping to retrieve songs");
			_logger.LogInformation("Attempting to retrieve songs");
			
			MusicStoreContext context = HttpContext
				.RequestServices
				.GetService(typeof(MusicStoreContext)) as MusicStoreContext;

			songs = context.GetAllSongs();

			if (songs.Count > 0)
			{
				return Ok(songs);
			}
			else
			{
				return NotFound();
			}
        	}

		[HttpGet("{id}")]
		public IActionResult Get(int id)
		{
			MusicStoreContext context = HttpContext
				.RequestServices
				.GetService(typeof(MusicStoreContext)) as MusicStoreContext;
			
			Song song = context.GetSong(id);

			if (song.Id != 0)
			{
				return Ok(song);
			}
			else
			{
				return NotFound();
			}
		}


        	[HttpPut("{id}")]
		public IActionResult Put(int id, [FromBody] Song song)
        	{
			MusicStoreContext context = HttpContext
				.RequestServices
				.GetService(typeof(MusicStoreContext)) as MusicStoreContext;

			song.Id = id;
			Console.WriteLine("Retrieving filepath of song");
			_logger.LogInformation("Retrieving filepath of song");
			var oldSongRecord = context.GetSong(id);
			song.SongPath = oldSongRecord.SongPath;

			MetadataRetriever updateMetadata = new MetadataRetriever();
			updateMetadata.UpdateMetadata(song, oldSongRecord);

			var updatedSong = updateMetadata.UpdatedSongRecord;
			context.UpdateSong(updatedSong);

			SongResult songResult = new SongResult
			{
				Message = updateMetadata.Message,
				SongTitle = updatedSong.Title
			};

			return Ok(songResult);
        	}
    	}
}
