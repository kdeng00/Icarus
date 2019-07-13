using System;
using System.Collections.Generic;
using System.IO;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Controllers.Managers;
using Icarus.Database.Repositories;
using Icarus.Models;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/coverart")]
    [ApiController]
    public class CoverArtController : ControllerBase
    {
        #region Fields
        private readonly ILogger<CoverArtController> _logger;
        #endregion


        #region Constructors
        public CoverArtController(ILogger<CoverArtController> logger)
        {
            _logger = logger;
        }
        #endregion


        #region HTTP Routes
        [HttpGet("{id}")]
        [Authorize("download:cover_art")]
        public IActionResult Get(int id)
        {
            var coverArt = new CoverArt
            {
                CoverArtId = id
            };

            var coverArtRepository = HttpContext
                .RequestServices
                .GetService(
                        typeof(CoverArtRepository)) as CoverArtRepository;

            coverArt = coverArtRepository.GetCoverArt(coverArt);

            if (coverArt != null)
            {
                var coverArtBytes = System.IO.File.ReadAllBytes(
                        coverArt.ImagePath);
                return File(coverArtBytes, "application/x-msdownload", 
                        coverArt.SongTitle);
            }
            else
                return NotFound();
        }
        #endregion
    }
}
