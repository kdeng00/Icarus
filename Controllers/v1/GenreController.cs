using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;

using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;

[Route("api/v1/genre")]
[ApiController]
[Authorize]
public class GenreController : BaseController
{
    #region Fields
    private readonly ILogger<GenreController>? _logger;
    private string? _connectionString;
    #endregion


    #region Properties
    #endregion


    #region Constructors
    public GenreController(ILogger<GenreController> logger, IConfiguration config)
    {
        _logger = logger;
        _config = config;
        _connectionString = _config.GetConnectionString("DefaultConnection");
    }
    #endregion


    #region HTTP Routes
    [HttpGet]
    public IActionResult GetGenres()
    {
        var genreStore = new GenreContext(_connectionString!);

        var genres = genreStore!.Genres!.ToList();

        if (genres.Count > 0)
        {
            return Ok(genres);
        }
        else
        {
            return NotFound(new List<Genre>());
        }
    }

    [HttpGet("{id}")]
    public IActionResult GetGenre(int id)
    {
        var genre = new Genre { Id = id };

        var genreStore = new GenreContext(_connectionString!);

        if (genreStore.DoesRecordExist(genre))
        {
            genre =  genreStore.RetrieveRecord(genre);

            return Ok(genre);
        }
        else
        {
            return NotFound(new Genre());
        }
    }
    #endregion
}
