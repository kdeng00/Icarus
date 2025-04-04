using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;

[Route("api/v1/song/stream")]
[ApiController]
[Authorize]
public class SongStreamController : BaseController
{
    #region Fields
    private string? _connectionString;
    private ILogger<SongStreamController>? _logger;
    #endregion


    #region Properties
    #endregion


    #region Constructor
    public SongStreamController(ILogger<SongStreamController> logger, IConfiguration config)
    {
        this._logger = logger;
        this._config = config;
        this._connectionString = this._config.GetConnectionString("DefaultConnection");
    }
    #endregion


    #region HTTP endpoints
    [HttpGet("{id}")]
    public async Task<IActionResult> StreamSong(Guid id)
    {
        var context = new SongContext(_config!.GetConnectionString("DefaultConnection")!);

        var song = context.Songs!.FirstOrDefault(sng => sng.Id == id);
        if (song == null)
        {
            return BadRequest();
        }
        var tokenManager = new Managers.TokenManager(this._config!);
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

        var stream = new FileStream(song!.SongPath(), FileMode.Open, FileAccess.Read);
        stream.Position = 0;
        var filename = song.Filename;

        if (string.IsNullOrEmpty(song.Filename))
        {
            filename = song.GenerateFilename();
        }

        _logger!.LogInformation("Starting to stream song...>");
        Console.WriteLine("Starting to streamsong...");

        return await Task.Run(() =>
        {
            return File(stream, "application/octet-stream", filename);
        });
    }
    #endregion
}
