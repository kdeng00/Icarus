using System;
using System.Collections.Generic;
using System.Linq;

using Microsoft.Extensions.Configuration;
using RestSharp;

using Icarus.Models;

namespace Icarus.Controllers.Managers
{
	public class TokenManager
	{
		#region Fields
		private IConfiguration _config;
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public TokenManager(IConfiguration config)
		{
			_config = config;
			InitializeValues();
		}
		#endregion


		#region Methods
		public LoginResult RetrieveLoginResult(User user)
		{
			// TODO: Request a token from Auth0

			return new LoginResult
			{
				UserId = 0,
				Username = user.Username,
				Token = "gggg",
				Expiration = 500
			};
		}

		private void InitializeValues()
		{
			// TODO: implement parse values necessary to 
			// retrieve a token from the appSettings using
			// the _config object
		}
		#endregion
	}
}
