using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;

[Route("api/v1/album")]
[ApiController]
[Authorize]
public class AlbumController : BaseController
{
    #region Fields
    private readonly ILogger<AlbumController>? _logger;
    private string? _connectionString;
    #endregion


    #region Properties
    #endregion


    #region Constructors
    public AlbumController(ILogger<AlbumController> logger, IConfiguration config)
    {
        _logger = logger;
        _config = config;
        _connectionString = _config.GetConnectionString("DefaultConnection");
    }
    #endregion


    #region HTTP Routes
    [HttpGet]
    public IActionResult GetAlbums()
    {
        var albumContext = new AlbumContext(_connectionString!);

        var albums = albumContext.Albums!.ToList();

        if (albums.Count > 0)
            return Ok(albums);
        else
            return NotFound();
    }

    [HttpGet("{id}")]
    public IActionResult GetAlbum(Guid id)
    {
        Album album = new Album { Id = id };

        var albumContext = new AlbumContext(_connectionString!);

        if (albumContext.DoesRecordExist(album))
        {
            album = albumContext.RetrieveRecord(album);

            return Ok(album);
        }
        else
            return NotFound();
    }
    #endregion
}
