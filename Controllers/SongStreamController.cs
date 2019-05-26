using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http.Headers;
using System.Web;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
	[Route("api/song/stream")]
	[ApiController]
	public class SongStreamController : ControllerBase
	{
		#region Fields
		private ILogger<SongStreamController> _logger;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongStreamController(ILogger<SongStreamController> logger)
		{
			_logger = logger;
		}
		#endregion


		#region HTTP endpoints
		[HttpGet("{id}")]
		public async Task<IActionResult> Get(int id)
		{
			var songStore= HttpContext
				.RequestServices
				.GetService(typeof(MusicStoreContext)) as MusicStoreContext;

			var song = songStore.GetSong(new Song { Id = id });

			var mem =  new MemoryStream();

			using (var stream = new FileStream(song.SongPath, FileMode.Open, FileAccess.Read))
			{
				await stream.CopyToAsync(mem);
			}

			mem.Position = 0;

			_logger.LogInformation("Starting to stream song...>");
			Console.WriteLine("Starting to streamsong...");

			return File(mem, "application/octet-stream", Path.GetFileName(song.SongPath));
		}
		#endregion
	}
}
