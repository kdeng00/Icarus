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

using Icarus.Controllers.Managers;
using Icarus.Models;
using Icarus.Database.Contexts;
using Icarus.Database.Repositories;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/song/data")]
    [ApiController]
    public class SongDataController : ControllerBase
    {
        #region Fields
        private SongRepository _songRepository;
        private AlbumRepository _albumRepository;
        private ArtistRepository _artistRepository;
        private GenreRepository _genreRepository;
        private YearRepository _yearRepository;
        private CoverArtRepository _coverArtRepository;
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
            _logger = logger;
            _songTempDir = _config.GetValue<string>("TemporaryMusicPath");
            _songMgr = new SongManager(config, _songTempDir);
        }
        #endregion

        private void Initialize()
        {
            _songRepository = HttpContext
                .RequestServices
                .GetService
                (typeof(SongRepository)) as SongRepository;

            _albumRepository = HttpContext
                .RequestServices
                .GetService(typeof(AlbumRepository)) as AlbumRepository;

            _artistRepository = HttpContext
                .RequestServices
                .GetService(typeof(ArtistRepository)) as ArtistRepository;

            _genreRepository = HttpContext
                .RequestServices
                .GetService(typeof(GenreRepository)) as GenreRepository;
        }


        [HttpGet("{id}")]
        [Route("private-scoped")]
        [Authorize("download:songs")]
        public async Task<IActionResult> Get(int id)
        {
            SongRepository context = HttpContext.RequestServices
                .GetService(typeof(SongRepository)) as SongRepository;
            var songMetaData = context.GetSong(id); 
            
            SongData song = await _songMgr.RetrieveSong(songMetaData);
            
            return File(song.Data, "application/x-msdownload", songMetaData.Filename);
        }

        [HttpPost]
        [Authorize("upload:songs")]
        public async Task Post([FromForm(Name = "file")] List<IFormFile> songData)
        {
            try
            {
                Initialize();

                YearRepository yearStore = HttpContext.RequestServices
                    .GetService(typeof(YearRepository)) as YearRepository;

                CoverArtRepository coverArtRepository = HttpContext
                    .RequestServices
                    .GetService
                    (typeof(CoverArtRepository)) as CoverArtRepository;

                Console.WriteLine("Uploading song...");
                _logger.LogInformation("Uploading song...");

                var uploads = _songTempDir;
                Console.WriteLine($"Song Root Path {uploads}");
                _logger.LogInformation($"Song root path {uploads}");
                foreach (var sng in songData)
                    if (sng.Length > 0) {
                        Console.WriteLine($"Song filename {sng.FileName}");
                        _logger.LogInformation($"Song filename {sng.FileName}");

                        await _songMgr.SaveSongToFileSystem(sng, _songRepository,
                            	_albumRepository, _artistRepository,
                            	_genreRepository, yearStore, coverArtRepository);
                    }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.LogError(msg, "An error occurred");
            }
        }

        [HttpDelete("{id}")]
        [Authorize("delete:songs")]
        public IActionResult Delete(int id)
        {
            SongRepository context = HttpContext.RequestServices
                .GetService(typeof(SongRepository)) as SongRepository;

            AlbumRepository albumStore = HttpContext.RequestServices
                .GetService(typeof(AlbumRepository)) as AlbumRepository;

            ArtistRepository artistStore = HttpContext.RequestServices
                .GetService(typeof(ArtistRepository)) as ArtistRepository;

            GenreRepository genreStore = HttpContext.RequestServices
                .GetService(typeof(GenreRepository)) as GenreRepository;

            YearRepository yearStore = HttpContext.RequestServices
                .GetService(typeof(YearRepository)) as YearRepository;
            
            CoverArtRepository coverArtRepository = HttpContext.RequestServices
                .GetService(typeof(CoverArtRepository)) as CoverArtRepository;

            var songMetaData = new Song{ Id = id };
            Console.WriteLine($"Id {songMetaData.Id}");
            songMetaData = context.GetSong(songMetaData);

            if (string.IsNullOrEmpty(songMetaData.Title))
            {
                _logger.LogInformation("Song does not exist");
                return NotFound("Song does not exist");
            }
            else
            {
                _logger.LogInformation("Starting process of deleting song from the filesystem and database");

                _songMgr.DeleteSong(songMetaData, context, albumStore, 
                        artistStore, genreStore, yearStore);

                return Ok();
            }
        }
    }
}
