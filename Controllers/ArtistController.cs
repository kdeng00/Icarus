using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
	[Route("api/artist")]
	[ApiController]
	public class ArtistController : ControllerBase
	{
		#region Fields
		private readonly ILogger<ArtistController> _logger;
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public ArtistController(ILogger<ArtistController> logger)
		{
			_logger = logger;
		}
		#endregion


		#region HTTP Routes
		[HttpGet]
		[Authorize("read:artists")]
		public IActionResult Get()
		{
			ArtistStoreContext artistStoreContext = HttpContext
				.RequestServices
				.GetService(typeof(ArtistStoreContext)) as ArtistStoreContext;

			var artists = artistStoreContext.GetArtists();

			if (artists.Count > 0)
			{
				return Ok(artists);
			}
			else
			{
				return NotFound();
			}
		}

		[HttpGet("{id}")]
		[Authorize("read:artists")]
		public IActionResult Get(int id)
		{
			Artist artist = new Artist
			{
				ArtistId = id
			};
			
			ArtistStoreContext artistStoreContext = HttpContext
				.RequestServices
				.GetService(typeof(ArtistStoreContext)) as ArtistStoreContext;

			if (artistStoreContext.DoesArtistExist(artist))
			{
				artist = artistStoreContext.GetArtist(artist);


				return Ok(artist);
			}
			else
			{
				return NotFound();
			}
		}
		#endregion
	}
}
