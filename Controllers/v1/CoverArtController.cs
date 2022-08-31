using System;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Controllers.Managers;
using Icarus.Database.Contexts;
using Icarus.Models;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/coverart")]
    [ApiController]
    public class CoverArtController : BaseController
    {
        #region Fields
        private readonly ILogger<CoverArtController> _logger;
        private string _connectionString;
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
        public IActionResult Get()
        {
            if (!IsTokenValid("read:songs"))
            {
                return StatusCode(401, "Not allowed");
            }

            var coverArtContext = new CoverArtContext(_connectionString);

            var coverArtRecords = coverArtContext.CoverArtImages.ToList();

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
        public async Task<IActionResult> Get(int id)
        {
            if (!IsTokenValid("download:cover_art"))
            {
                return StatusCode(401, "Not allowed");
            }

            var coverArt = new CoverArt { CoverArtID = id };

            var coverArtContext = new CoverArtContext(_connectionString);

            coverArt = coverArtContext.RetrieveRecord(coverArt);

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
