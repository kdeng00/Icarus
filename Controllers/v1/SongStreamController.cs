using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net.Http.Headers;
using System.Web;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Controllers.Managers;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/song/stream")]
    [ApiController]
    public class SongStreamController : BaseController
    {
        #region Fields
        private ILogger<SongStreamController> _logger;
        private string _connectionString;
        #endregion


        #region Properties
        #endregion


        #region Constructor
        public SongStreamController(ILogger<SongStreamController> logger, IConfiguration config)
        {
            _logger = logger;
            _config = config;
            _connectionString = _config.GetConnectionString("DefaultConnection");
        }
        #endregion


        #region HTTP endpoints
        [HttpGet("{id}")]
        public async Task<IActionResult> Get(int id)
        {
            if (!IsTokenValid("stream:songs"))
            {
                return StatusCode(401, "Not allowed");
            }

            var context = new SongContext(_config.GetConnectionString("DefaultConnection"));

            var song = context.Songs.FirstOrDefault(sng => sng.SongID == id);

            var stream = new FileStream(song.SongPath(), FileMode.Open, FileAccess.Read);
            stream.Position = 0;
            var filename = $"{song.Title}.mp3";

            _logger.LogInformation("Starting to stream song...>");
            Console.WriteLine("Starting to streamsong...");

            var file = await Task.Run(() => {
                return File(stream, "application/octet-stream", filename);
            });

            return file;
        }
        #endregion
    }
}
