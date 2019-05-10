using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;

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
		public IActionResult Get()
		{
			List<Artist> artists = new List<Artist>();
			// TODO: Implement functionality

			return Ok(artists);
		}

		[HttpGet("{id}")]
		public IActionResult Get(int id)
		{
			Artist artist = new Artist();
			// TODO: Implement functionality

			return Ok(artist);
		}
		#endregion
	}
}
