﻿using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

using Icarus.Controllers.Managers;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;



[Route("api/v1/song/data")]
[ApiController]
[Authorize]
public class SongDataController : BaseController
{
    #region Fields
    private string? _connectionString;
    private ILogger<SongDataController>? _logger;
    private SongManager? _songMgr;
    private string? _songTempDir;
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
        _songMgr = new SongManager(config, _songTempDir!);
    }
    #endregion



    [HttpGet("download/{id}")]
    public IActionResult Download(Guid id, [FromQuery] bool? randomizeFilename)
    {
        var tokenManager = new TokenManager(this._config!);
        var songContext = new SongContext(this._connectionString!);
        var accLvlContext = new AccessLevelContext(this._connectionString!);
        var songMetaData = songContext.RetrieveRecord(new Song { Id = id });
        var accessLevel = accLvlContext.GetAccessLevel(songMetaData.Id);
        var token = tokenManager.GetBearerToken(HttpContext);
        if (token == null || accessLevel == null)
        {
            return BadRequest();
        }

        if (!tokenManager.CanAccessSong(token, songMetaData, accessLevel))
        {
            return BadRequest();
        }

        var song = _songMgr!.RetrieveSong(songMetaData).Result;
        string filename;

        switch (songMetaData.AudioType)
        {
            case "wav":
                filename = DirectoryManager.GenerateDownloadFilename(10, Constants.FileExtensions.WAV_EXTENSION,
                    songMetaData.Title!, randomizeFilename);
                break;
            case "flac":
                filename = DirectoryManager.GenerateDownloadFilename(10, Constants.FileExtensions.FLAC_EXTENSION,
                    songMetaData.Title!, randomizeFilename);
                break;
            default:
                filename = DirectoryManager.GenerateDownloadFilename(10, Constants.FileExtensions.DEFAULT_AUDIO_EXTENSION,
                    songMetaData.Title!, randomizeFilename);
                break;
        }

        return File(song.Data!, "application/x-msdownload", filename);
    }

    // NOTE: No longer being used
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
    [Obsolete("Support for this endoint is no longer supported")]
    public IActionResult Upload([FromForm(Name = "file")] List<IFormFile> songData)
    {
        return new StatusCodeResult(StatusCodes.Status410Gone);
    }

    // The client is expected to send the file, metadata, and cover art separately.
    // Any metadata already on the file will be overwritten with values from the metadata
    // as well as the cover art
    //
    [HttpPost("upload/with/data")]
    public IActionResult UploadWithData([FromForm] UploadSongWithDataForm up)
    {
        try
        {
            if (up.SongData!.Length > 0 && up.CoverArtData!.Length > 0 && !string.IsNullOrEmpty(up.SongFile))
            {
                var meta = new Utilities.MetadataRetriever();
                var tmpSong = this._songMgr!.SaveSongTemp(up.SongData).Result;
                if (!meta.IsSupportedFile(tmpSong.SongPath()) && !meta.IsSupportedFile(up.CoverArtData))
                {
                    return BadRequest("Media is not supported");
                }
                else if (!meta.IsSupportedFile(tmpSong.SongPath()))
                {
                    return BadRequest("Song is not supported");
                }
                else if (!meta.IsSupportedFile(up.CoverArtData))
                {
                    return BadRequest("Cover art is not supported");
                }

                var song = Newtonsoft.Json.JsonConvert.DeserializeObject<Song>(up.SongFile);
                var tokMgr = new TokenManager(this._config!);
                var accessToken = Request.Headers["Authorization"];
                var userId = tokMgr.RetrieveUserIdFromToken(accessToken!);

                if (userId != null)
                {
                    song!.UserId = userId.Value;
                }

                _logger!.LogInformation($"Song title: {song!.Title}");

                song.Filename = tmpSong.Filename;
                song.SongDirectory = tmpSong.SongDirectory;
                song.DateCreated = tmpSong.DateCreated;
                song.AudioType = meta.FileExtensionType(tmpSong.SongPath());

                switch (song.AudioType)
                {
                    case "wav":
                        var _ = _songMgr.DeleteSongFromFileSystem(tmpSong);
                        return BadRequest(new UploadSongWithDataResponse
                        {
                            Subject = "No longer supported",
                            Message = "No support for .wav files",
                            Songs = new List<Song>()
                        });
                    case "flac":
                        song = _songMgr.SaveFlacSongToFileSystem(up.SongData, up.CoverArtData, song);
                        break;
                    default:
                        return BadRequest();
                }

                return Ok(song);
            }
        }
        catch (Exception ex)
        {
            _logger!.LogError(ex.Message, "An error occurred");
        }

        return BadRequest();
    }

    [HttpDelete("delete/{id}")]
    public IActionResult DeleteSong(Guid id)
    {
        var songContext = new SongContext(_connectionString!);
        var songMetaData = songContext.RetrieveRecord(new Song { Id = id });

        var tokenManager = new TokenManager(this._config!);
        var accLvlContext = new AccessLevelContext(this._connectionString!);
        var accessLevel = accLvlContext.GetAccessLevel(songMetaData.Id);
        var token = tokenManager.GetBearerToken(HttpContext);
        if (token == null || accessLevel == null)
        {
            return BadRequest();
        }

        if (!tokenManager.CanAccessSong(token, songMetaData, accessLevel))
        {
            return BadRequest();
        }

        if (string.IsNullOrEmpty(songMetaData.Title))
        {
            _logger!.LogInformation("Song does not exist");
            return NotFound("Song does not exist");
        }
        else
        {
            _logger!.LogInformation("Starting process of deleting song from the filesystem and database");

            _songMgr!.DeleteSong(songMetaData);

            return Ok(songMetaData);
        }
    }

    public class UploadSongWithDataForm
    {
        [FromForm(Name = "file")]
        public IFormFile? SongData { get; set; }
        // NOTE: Think about making this optional and if it is not provided, use the stock cover art
        [FromForm(Name = "cover")]
        public IFormFile? CoverArtData { get; set; }
        [FromForm(Name = "metadata")]
        public string? SongFile { get; set; }
    }

    public class UploadSongWithDataResponse
    {
        #region Properties
        [Newtonsoft.Json.JsonProperty("message")]
        public string? Message { get; set; }
        [Newtonsoft.Json.JsonProperty("subject")]
        public string? Subject { get; set; }
        [Newtonsoft.Json.JsonProperty("data")]
        public List<Song>? Songs { get; set; }
        #endregion
    }
}
