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
    private ILogger<SongStreamController>? _logger;
    #endregion


    #region Properties
    #endregion


    #region Constructor
    public SongStreamController(ILogger<SongStreamController> logger, IConfiguration config)
    {
        _logger = logger;
        _config = config;
    }
    #endregion


    #region HTTP endpoints
    [HttpGet("{id}")]
    public async Task<IActionResult> StreamSong(int id)
    {
        var context = new SongContext(_config!.GetConnectionString("DefaultConnection")!);

        var song = context.Songs!.FirstOrDefault(sng => sng.Id == id);

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
