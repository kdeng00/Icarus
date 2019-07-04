using System;
using System.Collections.Generic;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Database.Repositories;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/genre")]
    [ApiController]
    public class GenreController : ControllerBase
    {
        #region Fields
        private readonly ILogger<GenreController> _logger;
        #endregion


        #region Properties
        #endregion


        #region Constructors
        public GenreController(ILogger<GenreController> logger)
        {
            _logger = logger;
        }
        #endregion


        #region HTTP Routes
        [HttpGet]
        [Authorize("read:genre")]
        public IActionResult Get()
        {
            var genres = new List<Genre>();

            var genreStore = HttpContext.RequestServices
                .GetService(typeof(GenreRepository)) as GenreRepository;

            genres = genreStore.GetGenres();

            if (genres.Count > 0)
                return Ok(genres);
            else
                return NotFound(new List<Genre>());
        }

        [HttpGet("{id}")]
        [Authorize("read:genre")]
        public IActionResult Get(int id)
        {
            var genre = new Genre
            {
                GenreId = id
            };

            var genreStore = HttpContext.RequestServices
                .GetService(typeof(GenreRepository)) as GenreRepository;

            if (genreStore.DoesGenreExist(genre))
            {
                genre =  genreStore.GetGenre(genre);

                return Ok(genre);
            }
            else
                return NotFound(new Genre());
        }
        #endregion
    }
}
