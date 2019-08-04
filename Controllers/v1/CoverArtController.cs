using System;
using System.Collections.Generic;
using System.IO;
using System.Threading.Tasks;

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
        public IActionResult Get()
        {
            var coverArtRepository = HttpContext
                .RequestServices
                .GetService(
                        typeof(CoverArtRepository)) as CoverArtRepository;

            var coverArtRecords = coverArtRepository.GetCoverArtRecords();

            if (coverArtRecords == null)
            {
                _logger.LogInformation("No cover art records");
                return NotFound();
            }
            else
            {
                _logger.LogInformation("Found cover art records");
                return Ok(coverArtRecords);
            }

        }

        [HttpGet("{id}")]
        [Authorize("download:cover_art")]
        public async Task<IActionResult> Get(int id)
        {
            var coverArt = new CoverArt { CoverArtId = id };

            var coverArtRepository = HttpContext
                .RequestServices
                .GetService(
                        typeof(CoverArtRepository)) as CoverArtRepository;

            coverArt = coverArtRepository.GetCoverArt(coverArt);

            if (coverArt != null)
            {
                _logger.LogInformation("Found cover art record");
                var coverArtBytes = await System.IO.File.ReadAllBytesAsync(
                        coverArt.ImagePath);

                return File(coverArtBytes, "application/x-msdownload", 
                        coverArt.SongTitle);
            }
            else
            {
                _logger.LogInformation("Cover art not found");
                return NotFound();
            }
        }
        #endregion
    }
}
