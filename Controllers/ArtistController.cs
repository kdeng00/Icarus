using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Models;

namespace Icarus.Controllers
{
	[Route("api/artist")]
	[ApiController]
	public class ArtistController : ControllerBase
	{
		[HttpGet]
		public IActionResult Get()
		{
			List<Artist> artists = new List<Artist>();

			return Ok(artists);
		}

		[HttpGet("{id}")]
		public IActionResult Get(int id)
		{
			Artist artist = new Artist();

			return Ok(artist);
		}

		[HttpDelete("{id}")]
		public IActionResult Delete(int id)
		{
			return Ok();
		}
	}
}
