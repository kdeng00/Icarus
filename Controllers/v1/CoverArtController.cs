using System;
using System.Collections.Generic;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/coverart/song")]
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
	public IActionResult Get(int id)
	{
	    return NotFound();
	}
        #endregion
    }
}
