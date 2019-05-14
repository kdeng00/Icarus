using System;
using System.Collections.Generic;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
	// TODO: Implement Genre API functionality #41
	[Route("api/genre")]
	[ApiController]
	public class GenreController : ControllerBase
	{
		#region Fields
		private readonly ILogger<GenreController> _logger;
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public GenreController(ILogger<GenreController> logger)
		{
			_logger = logger;
		}
		#endregion


		#region HTTP Routes
		[HttpGet]
		public IActionResult Get()
		{
			var genres = new List<Genre>();

			return Ok(genres);
		}

		[HttpGet("{id}")]
		public IActionResult Get(int id)
		{
			var genre = new Genre
			{
				GenreId = id
			};

			return Ok(genre);
		}
		#endregion
	}
}
