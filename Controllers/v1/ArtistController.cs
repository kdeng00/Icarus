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

namespace Icarus.Controllers.V1
{
    [Route("api/v1/artist")]
    [ApiController]
    public class ArtistController : ControllerBase
    {
        #region Fields
        private readonly ILogger<ArtistController> _logger;
        private string _connectionString;
        private IConfiguration _config;
        #endregion


        #region Properties
        #endregion


        #region Constructors
        public ArtistController(ILogger<ArtistController> logger, IConfiguration config)
        {
            _logger = logger;
            _config = config;
            _connectionString = _config.GetConnectionString("DefaultConnection");
        }
        #endregion


        #region HTTP Routes
        [HttpGet]
        [Authorize("read:artists")]
        public IActionResult Get()
        {
            var artistContext = new ArtistContext(_connectionString);

            var artists = artistContext.Artists.ToList();

            if (artists.Count > 0)
                return Ok(artists);
            else
                return NotFound();
        }

        [HttpGet("{id}")]
        [Authorize("read:artists")]
        public IActionResult Get(int id)
        {
            Artist artist = new Artist
            {
                ArtistID = id
            };
            
            var artistContext = new ArtistContext(_connectionString);

            if (artistContext.DoesRecordExist(artist))
            {
                artist = artistContext.RetrieveRecord(artist);

                return Ok(artist);
            }
            else
                return NotFound();
        }
        #endregion
    }
}
