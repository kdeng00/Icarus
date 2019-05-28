using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Repositories;

namespace Icarus.Controllers.V1
{
	[Route("api/v1/login")]
	[ApiController]
	public class LoginController : ControllerBase
	{
		#region Fields
		private IConfiguration _config;
		private ILogger<LoginController> _logger;
		#endregion


		#region Properties
		#endregion


		#region Contructors
		public LoginController(IConfiguration config, ILogger<LoginController> logger)
		{
			_config = config;
			_logger = logger;
		}
		#endregion


		#region HTTP endpoints
		public IActionResult Post([FromBody] User user)
		{
			UserRepository context = HttpContext
				.RequestServices
				.GetService(typeof(UserRepository)) as UserRepository;

			_logger.LogInformation("Starting process of validating credentials");
			
			var message = "Invalid credentials";
			var password = user.Password;

			var loginRes = new LoginResult
			{
				Username = user.Username
			};

			if (context.DoesUserExist(user))
			{
				user = context.RetrieveUser(user);

				var validatePass = new PasswordEncryption();
				var validated = validatePass.VerifyPassword(user, password);
				if (!validated)
				{
					loginRes.Message = message;
					_logger.LogInformation(message);

					return Ok(loginRes);
				}

				_logger.LogInformation("Successfully validated user credentials");

				TokenManager tk = new TokenManager(_config);

				loginRes = tk.RetrieveLoginResult(user);

				return Ok(loginRes);
			}
			else
			{
				loginRes.Message = message;

				return NotFound(loginRes);
			}
		}
		#endregion
	}
}
