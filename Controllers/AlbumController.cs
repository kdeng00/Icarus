using System;
using System.Collections.Generic;
using System.Configuration;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;

namespace Icarus.Controllers
{
	[Route("api/album")]
	[ApiController]
	public class AlbumController : ControllerBase
	{
		#region Fields
		private readonly ILogger<AlbumController> _logger;
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public AlbumController(ILogger<AlbumController> logger)
		{
			_logger = logger;
		}
		#endregion


		#region HTTP Routes
		[HttpGet]
		public IActionResult Get()
		{
			List<Album> albums = new List<Album>();

			return Ok(albums);
		}

		[HttpGet("{id}")]
		public IActionResult Get(int id)
		{
			Album album = new Album();

			return Ok(album);
		}
		#endregion
	}
}
