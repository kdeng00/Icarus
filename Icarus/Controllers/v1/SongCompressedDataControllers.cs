using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;
using Icarus.Controllers.Managers;

namespace Icarus.Controllers.V1;

[Route("api/v1/song/compressed/data")]
[ApiController]
[Authorize]
public class SongCompressedDataController : BaseController
{
    #region Fields
    private string? _connectionString;
    private string? _songTempDir;
    private string? _archiveDir;
    #endregion


    #region Properties
    #endregion


    #region Constructor
    public SongCompressedDataController(IConfiguration config)
    {
        _config = config;
        _songTempDir = _config.GetValue<string>("TemporaryMusicPath");
        _archiveDir = _config.GetValue<string>("ArchivePath");
        _connectionString = _config.GetConnectionString("DefaultConnection");
    }
    #endregion


    #region API Routes
    [HttpGet("{id}")]
    public async Task<IActionResult> DownloadCompressedSong(Guid id, [FromQuery] bool? randomizeFilename)
    {
        var context = new SongContext(_connectionString!);

        SongCompression cmp = new SongCompression(_archiveDir!);

        Console.WriteLine($"Archive directory root: {_archiveDir}");

        Console.WriteLine("Starting process of retrieving comrpessed song");
        var sng = context.RetrieveRecord(new Song { Id = id });

        var tokenManager = new TokenManager(this._config!);
        var accLvlContext = new AccessLevelContext(this._connectionString!);
        var accessLevel = accLvlContext.GetAccessLevel(sng.Id);
        var token = tokenManager.GetBearerToken(HttpContext);
        if (token == null || accessLevel == null)
        {
            return BadRequest();
        }

        if (!tokenManager.CanAccessSong(token, sng, accessLevel))
        {
            return BadRequest();
        }
        SongData song = await cmp.RetrieveCompressedSong(sng);

        var filename = DirectoryManager.GenerateDownloadFilename(10, Constants.FileExtensions.ZIP_EXTENSION, sng.Title!, randomizeFilename);

        return File(song.Data!, "application/x-msdownload", filename);
    }
    #endregion
}
