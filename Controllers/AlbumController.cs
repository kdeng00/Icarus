using System;
using System.Collections.Generic;
using System.Configuration;

using Microsoft.AspNetCore.Mvc;

using Icarus.Models;

namespace Icarus.Controllers
{
	[Route("api/album")]
	[ApiController]
	public class AlbumController : ControllerBase
	{
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

		[HttpDelete("{id}")]
		public IActionResult Delete(int id)
		{
			return Ok();
		}
	}
}
