using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
	[Route("api/login")]
	[ApiController]
	public class LoginController : ControllerBase
	{
		#region Fields
		private IConfiguration _config;
		#endregion


		#region Properties
		#endregion


		#region Contructors
		public LoginController(IConfiguration config)
		{
			_config = config;
		}
		#endregion


		#region HTTP endpoints
		public IActionResult Post([FromBody] User user)
		{
			// TODO: Secure this HTTP endpoint. #38
			// Currently there is no check done to determine whether or not the 
			// user's password sent with the request matches the password stored
			// in the database. In fact there is not check if the user credentials
			// sent with this request even exist in the database. I knowingly left
			// this bug in here for the sole purpose of making it easier to test
			// but it should now be addressed.

			UserStoreContext context = HttpContext
				.RequestServices
				.GetService(typeof(UserStoreContext)) as UserStoreContext;

			user = context.RetrieveUser(user);
			Console.WriteLine($"Username: {user.Username}");

			TokenManager tk = new TokenManager(_config);

			LoginResult loginRes = tk.RetrieveLoginResult(user);

			return Ok(loginRes);
		}
		#endregion
	}
}
