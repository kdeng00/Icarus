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
using Microsoft.Extensions.Logging;

using Icarus.Controllers.Managers;
using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
	[Route("api/song/data")]
	[ApiController]
	public class SongDataController : ControllerBase
	{
		#region Fields
        	private IConfiguration _config;
		private ILogger<SongDataController> _logger;
		private SongManager _songMgr;
		private string _songTempDir;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongDataController(IConfiguration config, ILogger<SongDataController> logger)
		{
			_config = config;
			_logger = logger;
			_songTempDir = _config.GetValue<string>("TemporaryMusicPath");
			_songMgr = new SongManager(config, _songTempDir);
		}
		#endregion


        	[HttpGet("{id}")]
		[Route("private-scoped")]
		[Authorize("download:songs")]
        	public async Task<IActionResult> Get(int id)
        	{
			MusicStoreContext context = HttpContext
				.RequestServices
				.GetService(typeof(MusicStoreContext)) as MusicStoreContext;
			var songMetaData = context.GetSong(id); 
			
			SongData song = await _songMgr.RetrieveSong(songMetaData);
			
			return File(song.Data, "application/x-msdownload", songMetaData.Filename);
		}

        	[HttpPost]
		[Authorize("upload:songs")]
        	public async Task Post([FromForm(Name = "file")] List<IFormFile> songData)
        	{
			try
			{
				MusicStoreContext songStoreContext = HttpContext
					.RequestServices
					.GetService(typeof(MusicStoreContext)) as MusicStoreContext;
				AlbumStoreContext albumStoreContext = HttpContext
					.RequestServices
					.GetService(typeof(AlbumStoreContext)) as AlbumStoreContext;
				ArtistStoreContext artistStoreContext = HttpContext
					.RequestServices
					.GetService(typeof(ArtistStoreContext)) as ArtistStoreContext;
				GenreStoreContext genreStore = HttpContext
					.RequestServices
					.GetService(typeof(GenreStoreContext)) as GenreStoreContext;
				YearStoreContext yearStore = HttpContext
					.RequestServices
					.GetService(typeof(YearStoreContext)) as YearStoreContext;

				Console.WriteLine("Uploading song...");
				_logger.LogInformation("Uploading song...");

				var uploads = _songTempDir;
				Console.WriteLine($"Song Root Path {uploads}");
				_logger.LogInformation($"Song root path {uploads}");
				foreach (var sng in songData)
				{
					if (sng.Length > 0) {
						Console.WriteLine($"Song filename {sng.FileName}");
						_logger.LogInformation($"Song filename {sng.FileName}");

						await _songMgr.SaveSongToFileSystem(sng, songStoreContext,
								albumStoreContext, artistStoreContext,
								genreStore, yearStore);
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.LogError(msg, "An error occurred");
			}
		}

        	[HttpDelete("{id}")]
		[Authorize("delete:songs")]
        	public void Delete(int id)
        	{
			MusicStoreContext context = HttpContext
				.RequestServices
				.GetService(typeof(MusicStoreContext)) as MusicStoreContext;
			
			var songMetaData = context.GetSong(id);
			
			var result = _songMgr.DeleteSongFromFileSystem(songMetaData);
			
			if (result)
			{
				context.DeleteSong(songMetaData.Id);
			}
		}
    	}
}
