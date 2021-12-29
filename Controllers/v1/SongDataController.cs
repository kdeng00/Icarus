using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;

using Icarus.Controllers.Managers;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/song/data")]
    [ApiController]
    public class SongDataController : ControllerBase
    {
        #region Fields
        private string _connectionString;
        private IConfiguration _config;
        private ILogger<SongDataController> _logger;
        private SongManager _songMgr;
        private string _songTempDir;
        #endregion


        #region Properties
        #endregion


        #region Constructor
        public SongDataController(IConfiguration config, ILogger<SongDataController> logger)
        {
            _config = config;
            _connectionString = _config.GetConnectionString("DefaultConnection");
            _logger = logger;
            _songTempDir = _config.GetValue<string>("TemporaryMusicPath");
            _songMgr = new SongManager(config, _songTempDir);
        }
        #endregion



        [HttpGet("download/{id}")]
        [Route("private-scoped")]
        [Authorize("download:songs")]
        public async Task<IActionResult> Get(int id)
        {
            var songContext = new SongContext(_connectionString);
            var songMetaData = songContext.RetrieveRecord(new Song { SongID = id});
            
            var song = await _songMgr.RetrieveSong(songMetaData);
            
            return File(song.Data, "application/x-msdownload", songMetaData.Filename);
        }

        // Assumes that the song already has metadata such as
        // Title
        // Artist
        // Album
        // Genre
        // Year
        // Track
        // Track count
        // Disc
        // Disc count
        // Cover art
        //
        [HttpPost("upload"), DisableRequestSizeLimit]
        [Route("private-scoped")]
        [Authorize("upload:songs")]
        public async Task<IActionResult> Post([FromForm(Name = "file")] List<IFormFile> songData)
        {
            try
            {
                Console.WriteLine("Uploading song...");
                _logger.LogInformation("Uploading song...");

                var uploads = _songTempDir;
                Console.WriteLine($"Song Root Path {uploads}");
                _logger.LogInformation($"Song root path {uploads}");

                foreach (var sng in songData)
                    if (sng.Length > 0) {
                        Console.WriteLine($"Song filename {sng.FileName}");
                        _logger.LogInformation($"Song filename {sng.FileName}");

                        await _songMgr.SaveSongToFileSystem(sng);
                    }

                return Ok();
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.LogError(msg, "An error occurred");
            }

            return NotFound();
        }

        // The client is expected to send the file, metadata, and cover art separately.
        // Any metadata already on the file will be overwritten with values from the metadata
        // as well as the cover art
        //
        [HttpPost("upload/with/data")]
        [Route("private-scoped")]
        [Authorize("upload:songs")]
        public async Task<IActionResult> Post ([FromForm] UploadSongWithDataForm up)
        {
            try
            {
                if (up.SongData.Length > 0 && up.CoverArtData.Length > 0 && !string.IsNullOrEmpty(up.SongFile))
                {
                    var song = Newtonsoft.Json.JsonConvert.DeserializeObject<Song>(up.SongFile);
                    _logger.LogInformation($"Song title: {song.Title}");

                    await _songMgr.SaveSongToFileSystem(up.SongData, up.CoverArtData, song);
                }
            }
            catch (Exception ex)
            {
                _logger.LogError(ex.Message, "An error occurred");
            }

            return Ok();
        }

        [HttpDelete("delete/{id}")]
        [Authorize("delete:songs")]
        public IActionResult Delete(int id)
        {
            var songContext = new SongContext(_connectionString);

            var songMetaData = new Song{ SongID = id };
            Console.WriteLine($"Id {songMetaData.SongID}");

            songMetaData = songContext.RetrieveRecord(songMetaData);

            if (string.IsNullOrEmpty(songMetaData.Title))
            {
                _logger.LogInformation("Song does not exist");
                return NotFound("Song does not exist");
            }
            else
            {
                _logger.LogInformation("Starting process of deleting song from the filesystem and database");

                _songMgr.DeleteSong(songMetaData);

                return Ok();
            }

        }

            public class UploadSongWithDataForm
            {
                [FromForm(Name = "file")]
                public IFormFile SongData { get; set; }
                // TODO: Think about making this optional and if it is not provided, use the stock cover art
                [FromForm(Name = "cover")]
                public IFormFile CoverArtData { get; set; }
                [FromForm(Name = "metadata")]
                public string SongFile { get; set; }
            }
    }
}
