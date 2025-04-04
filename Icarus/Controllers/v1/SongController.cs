using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

using Icarus.Controllers.Managers;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;

[Route("api/v1/song")]
[ApiController]
[Authorize]
public class SongController : BaseController
{
    #region Fields
    private readonly ILogger<SongController>? _logger;
    private string? _connectionString;
    private SongManager? _songMgr;
    #endregion


    #region Properties
    #endregion


    #region Constructor
    public SongController(IConfiguration config, ILogger<SongController> logger)
    {
        _config = config;
        _logger = logger;
        _connectionString = _config.GetConnectionString("DefaultConnection");
        _songMgr = new SongManager(config);
    }
    #endregion


    #region Methods
    #region HTTP Endpoints
    [HttpGet]
    public IActionResult GetSongs()
    {
        Console.WriteLine("Attemtping to retrieve songs");
        _logger!.LogInformation("Attempting to retrieve songs");

        var context = new SongContext(_connectionString!);

        var songs = context.Songs!.ToList();

        if (songs.Count > 0)
        {
            return Ok(songs);
        }
        else
        {
            return NotFound();
        }
    }

    [HttpGet("{id}")]
    public IActionResult GetSong(Guid id)
    {
        var context = new SongContext(_connectionString!);

        var song = context.RetrieveRecord(new Song { Id = id });

        Console.WriteLine("Here");

        if (song.Id.ToString().Length != 0)
            return Ok(song);
        else
            return NotFound();
    }

    [HttpPut("{id}")]
    public IActionResult UpdateSong(Guid id, [FromBody] Song song)
    {
        song.Id = id;
        Console.WriteLine("Retrieving filepath of song");
        _logger!.LogInformation("Retrieving filepath of song");

        if (!_songMgr!.DoesSongExist(song))
        {
            return NotFound(new SongResult
            {
                Message = "Song does not exist"
            });
        }

        var tokenManager = new TokenManager(this._config!);
        var accLvlContext = new AccessLevelContext(this._connectionString!);
        var accessLevel = accLvlContext.GetAccessLevel(song.Id);
        var token = tokenManager.GetBearerToken(HttpContext);
        if (token == null || accessLevel == null)
        {
            return BadRequest();
        }

        if (!tokenManager.CanAccessSong(token, song, accessLevel))
        {
            return BadRequest();
        }

        var songRes = _songMgr.UpdateSong(song);

        return Ok(songRes);
    }
    #endregion
    #endregion
}
