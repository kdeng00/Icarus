using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/song")]
    [ApiController]
    public class SongController : ControllerBase
    {
        #region Fields
        private readonly ILogger<SongController> _logger;
        private string _connectionString;
        private IConfiguration _config;
        private SongManager _songMgr;
        #endregion


        #region Properties
        #endregion


        #region Constructor
        public SongController(IConfiguration config, ILogger<SongController> logger)
        {
            _config = config;
            _connectionString = _config.GetConnectionString("DefaultConnection");
            _logger = logger;
            _songMgr = new SongManager(config);
        }
        #endregion


        #region Methods
        private string ParseBearerTokenFromHeader()
        {
            var token = string.Empty;
            const string tokenType = "Bearer";

            var req = Request;
            var auth = req.Headers.Authorization; 
            var val = auth.ToString();

            if (val.Contains(tokenType) && val.Split(" ").Count() > 1)
            {
                var split = val.Split(" ");
                token = split[1];
            }


            return token;
        }
        #region HTTP Endpoints


        [HttpGet]
        // [Authorize("read:song_details")]
        public IActionResult Get()
        {
            var token = ParseBearerTokenFromHeader();
            var tokMgr = new TokenManager(_config);

            if (!tokMgr.IsTokenValid("read:song_details", token))
            {
                return StatusCode(401, "Not allowed");
            }

            List<Song> songs = new List<Song>();
            Console.WriteLine("Attemtping to retrieve songs");
            _logger.LogInformation("Attempting to retrieve songs");
            
            var context = new SongContext(_connectionString);

            songs = context.Songs.ToList();

            if (songs.Count > 0)
                return Ok(songs);
            else
                return NotFound();
        }

        [HttpGet("{id}")]
        [Authorize("read:song_details")]
        public IActionResult Get(int id)
        {
            var context = new SongContext(_connectionString);
            
            Song song = new Song { SongID = id };
            song = context.RetrieveRecord(song);

            Console.WriteLine("Here");

            if (song.SongID != 0)
                return Ok(song);
            else
                return NotFound();
        }

        [Authorize("update:songs")]
        [HttpPut("{id}")]
        public IActionResult Put(int id, [FromBody] Song song)
        {
            var context = new SongContext(_connectionString);

            song.SongID = id;
            Console.WriteLine("Retrieving filepath of song");
            _logger.LogInformation("Retrieving filepath of song");

            if (!_songMgr.DoesSongExist(song))
            {
                return NotFound(new SongResult
                {
                    Message = "Song does not exist"
                });
            }

            var songRes = _songMgr.UpdateSong(song);

            return Ok(songRes);
        }
        #endregion
        #endregion
    }
}
