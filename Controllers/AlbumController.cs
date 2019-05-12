using System;
using System.Collections.Generic;
using System.Configuration;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Models.Context;

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
		[Authorize("read:albums")]
		public IActionResult Get()
		{
			List<Album> albums = new List<Album>();

			AlbumStoreContext albumStoreContext = HttpContext
				.RequestServices
				.GetService(typeof(AlbumStoreContext)) as AlbumStoreContext;

			albums = albumStoreContext.GetAlbums();

			if (albums.Count > 0)
			{
				return Ok(albums);
			}
			else
			{
				return NotFound();
			}
		}

		[HttpGet("{id}")]
		[Authorize("read:albums")]
		public IActionResult Get(int id)
		{
			Album album = new Album
			{
				AlbumId = id
			};

			AlbumStoreContext albumStoreContext = HttpContext
				.RequestServices
				.GetService(typeof(AlbumStoreContext)) as AlbumStoreContext;

			if (albumStoreContext.DoesAlbumExist(album))
			{
				album = albumStoreContext.GetAlbum(album);

				return Ok(album);
			}
			else
			{
				return NotFound();
			}
		}
		#endregion
	}
}
