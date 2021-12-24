using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Database.Contexts;
// using Icarus.Database.Repositories;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/album")]
    [ApiController]
    public class AlbumController : ControllerBase
    {
        #region Fields
        private readonly ILogger<AlbumController> _logger;
        private string _connectionString;
        private IConfiguration _config;
        #endregion


        #region Properties
        #endregion


        #region Constructors
        public AlbumController(ILogger<AlbumController> logger, IConfiguration config)
        {
            _logger = logger;
            _config = config;
            _connectionString = _config.GetConnectionString("DefaultConnection");
        }
        #endregion


        #region HTTP Routes
        [HttpGet]
        [Authorize("read:albums")]
        public IActionResult Get()
        {
            List<Album> albums = new List<Album>();

            var albumContext = new AlbumContext(_connectionString);

            albums = albumContext.Albums.ToList();

            if (albums.Count > 0)
                return Ok(albums);
            else
                return NotFound();
        }

        [HttpGet("{id}")]
        [Authorize("read:albums")]
        public IActionResult Get(int id)
        {
            Album album = new Album
            {
                AlbumID = id
            };

            var albumContext = new AlbumContext(_connectionString);

            if (albumContext.DoesRecordExist(album))
            {
                album = albumContext.RetrieveRecord(album);

                return Ok(album);
            }
            else
                return NotFound();
        }
        #endregion
    }
}
