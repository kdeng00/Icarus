using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
	[Route("api/song")]
	[ApiController]
    	public class SongController : ControllerBase
    	{
		#region Fields
		private IConfiguration _config;
		private MusicStoreContext _context;
		private SongManager _songMgr;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongController(IConfiguration config)
		{
			_config = config;
			_songMgr = new SongManager(config);
		}
		#endregion


        	[HttpGet]
		[Authorize("read:song_details")]
        	public ActionResult<IEnumerable<Song>> Get()
        	{
			List<Song> songs = new List<Song>();
			Console.WriteLine("Attemtping to retrieve songs");
			
			MusicStoreContext context = HttpContext
										.RequestServices
										.GetService(typeof(MusicStoreContext)) 
										as MusicStoreContext;

			songs = context.GetAllSongs();

			return songs;
        	}

		[HttpGet("{id}")]
		public ActionResult<Song> Get(int id)
		{
			MusicStoreContext context = HttpContext
										.RequestServices
										.GetService(typeof(MusicStoreContext)) 
										as MusicStoreContext;
			
			Song song = context.GetSong(id);
			
			return song;
		}


        	[HttpPut("{id}")]
		public IActionResult Put(int id, [FromBody] Song song)
        	{
			// TODO: Implement updating of song metadata
			MusicStoreContext context = HttpContext.RequestServices
				.GetService(typeof(MusicStoreContext)) as MusicStoreContext;
			Console.WriteLine("Retrieving filepath of song");
			var songPath = context.GetSong(id).SongPath;
			song.SongPath = songPath;

			MetadataRetriever updateMetadata = new MetadataRetriever();
			updateMetadata.UpdateMetadata(song);

			context.UpdateSong(song);

			SongResult songResult = new SongResult();

			return Ok(songResult);
        	}

		[HttpDelete("{id}")]
		public void Delete(int id)
		{
			MusicStoreContext context = HttpContext
										.RequestServices
										.GetService(typeof(MusicStoreContext)) 
										as MusicStoreContext;
			
			context.DeleteSong(id);
        }
    }
}
