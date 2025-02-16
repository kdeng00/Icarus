using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

using Icarus.Controllers.Managers;
using Icarus.Database.Contexts;
using Icarus.Models;

namespace Icarus.Controllers.V1;

[Route("api/v1/coverart")]
[ApiController]
[Authorize]
public class CoverArtController : BaseController
{
    #region Fields
    private readonly ILogger<CoverArtController>? _logger;
    private string? _connectionString;
    #endregion


    #region Constructors
    public CoverArtController(ILogger<CoverArtController> logger, IConfiguration config)
    {
        _logger = logger;
        _config = config;
        _connectionString = _config.GetConnectionString("DefaultConnection");
    }
    #endregion


    #region HTTP Routes
    [HttpGet]
    public IActionResult GetCoverArts()
    {
        var coverArtContext = new CoverArtContext(_connectionString!);

        var coverArtRecords = coverArtContext.CoverArtImages!.ToList();

        if (coverArtRecords == null)
        {
            _logger!.LogInformation("No cover art records");
            return NotFound();
        }
        else
        {
            _logger!.LogInformation("Found cover art records");
            return Ok(coverArtRecords);
        }
    }

    [HttpGet("{id}")]
    public IActionResult GetCoverArt(int id)
    {
        var coverArt = new CoverArt { Id = id };

        var coverArtContext = new CoverArtContext(_connectionString!);

        coverArt = coverArtContext.RetrieveRecord(coverArt);

        if (coverArt != null)
        {
            _logger!.LogInformation("Found cover art record");
            var coverArtBytes = System.IO.File.ReadAllBytes(
                    coverArt.ImagePath());

            return File(coverArtBytes, "application/x-msdownload",
                    coverArt.SongTitle);
        }
        else
        {
            _logger!.LogInformation("Cover art not found");
            return NotFound();
        }
    }

    [HttpGet("data/download/{id}")]
    public async Task<IActionResult> Download(int id, [FromQuery] bool? randomizeFilename)
    {
        var songContext = new SongContext(_connectionString!);
        var covMgr = new CoverArtManager(this._config!);

        var songMetaData = songContext.RetrieveRecord(new Song { Id = id });
        var c = covMgr.GetCoverArt(songMetaData);

        var filename = DirectoryManager.GenerateDownloadFilename(10, Constants.FileExtensions.JPG_EXTENSION, songMetaData.Title!, randomizeFilename);

        var data = await c.GetData();

        return File(data, "application/x-msdownload", filename);
    }
    #endregion
}
