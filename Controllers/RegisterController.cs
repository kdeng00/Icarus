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
    	[Route("api/register")]
    	[ApiController]
    	public class RegisterController : ControllerBase
    	{
		#region Fields
		private IConfiguration _config;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public RegisterController(IConfiguration config)
		{
	    		_config = config;
		}
		#endregion

		[HttpPost]
        	public void Post([FromBody] User user)
        	{
			Console.WriteLine($"Username: {user.Username}");
			Console.WriteLine($"Password: {user.Password}");

			PasswordEncryption pe = new PasswordEncryption();
			user.Password = pe.HashPassword(user);
			user.EmailVerified = false;
			Console.WriteLine($"Hashed Password: {user.Password}");

			UserStoreContext context = HttpContext
				.RequestServices
				.GetService(typeof(UserStoreContext)) as UserStoreContext;

			context.SaveUser(user);
        	}
    	}
}
