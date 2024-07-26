using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;

[Route("api/v1/artist")]
[ApiController]
[Authorize]
public class ArtistController : BaseController
{
    #region Fields
    private readonly ILogger<ArtistController>? _logger;
    private string? _connectionString;
    #endregion


    #region Properties
    #endregion


    #region Constructors
    public ArtistController(ILogger<ArtistController> logger, IConfiguration config)
    {
        _logger = logger;
        _config = config;
        _connectionString = _config.GetConnectionString("DefaultConnection");
    }
    #endregion


    #region HTTP Routes
    [HttpGet]
    public IActionResult GetArtists()
    {
        var artistContext = new ArtistContext(_connectionString!);

        var artists = artistContext.Artists.ToList();

        if (artists.Count > 0)
            return Ok(artists);
        else
            return NotFound();
    }

    [HttpGet("{id}")]
    public IActionResult GetArtist(int id)
    {
        Artist artist = new Artist { Id = id };
        
        var artistContext = new ArtistContext(_connectionString!);

        if (artistContext.DoesRecordExist(artist))
        {
            artist = artistContext.RetrieveRecord(artist);

            return Ok(artist);
        }
        else
        {
            return NotFound();
        }
    }
    #endregion
}
