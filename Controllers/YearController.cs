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

			var yearStore = HttpContext
				.RequestServices
				.GetService(typeof(YearStoreContext)) as YearStoreContext;

			yearValues = yearStore.GetSongYears();

			if (yearValues.Count > 0)
			{
				return Ok(yearValues);
			}
			else
			{
				return NotFound(new List<Year>());
			}
		}

		[HttpGet("{id}")]
		public IActionResult Get(int id)
		{
			var year = new Year
			{
				YearId = id
			};

			var yearStore = HttpContext
				.RequestServices
				.GetService(typeof(YearStoreContext)) as YearStoreContext;

			if (yearStore.DoesYearExist(year))
			{
				year = yearStore.GetSongYear(year);

				return Ok(year);
			}
			else
			{
				return NotFound(new Year());
			}
		}
		#endregion
	}
}
