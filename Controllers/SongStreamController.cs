using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Logging;

using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
	[Route("api/song/stream")]
	[ApiController]
	public class SongStreamController : ControllerBase
	{
		#region Fields
		private ILogger<SongStreamController> _logger;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongStreamController(ILogger<SongStreamController> logger)
		{
			_logger = logger;
		}
		#endregion
	}
}
