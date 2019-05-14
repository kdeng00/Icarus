using System;
using System.Collections.Generic;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controller
{
	// TODO: Implemnt Year API functionality #42
	[Route("api/year")]
	[ApiController]
	public class YearController : ControllerBase
	{
		#region Fields
		private readonly ILogger<YearController> _logger;
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public YearController(ILogger<YearController> logger)
		{
			_logger = logger;
		}
		#endregion


		#region HTTP Routes
		[HttpGet]
		public IActionResult Get()
		{
			var yearValues = new List<Year>();

			return Ok();
		}

		[HttpGet("{id}")]
		public IActionResult Get(int id)
		{
			var year = new Year
			{
				YearId = id
			};

			return Ok(year);
		}
		#endregion
	}
}
