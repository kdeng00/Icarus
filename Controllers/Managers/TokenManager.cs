using System;
using System.Collections.Generic;
using System.Linq;

using Microsoft.Extensions.Configuration;
using Newtonsoft.Json;
using RestSharp;

using Icarus.Models;

namespace Icarus.Controllers.Managers
{
	public class TokenManager
	{
		#region Fields
		private IConfiguration _config;
		private string _clientId;
		private string _clientSecret;
		private string _audience;
		private string _grantType;
		private string _url;
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
			var client = new RestClient(_url);
			var request = new RestRequest("oauth/token", Method.POST);
			var tokenRequest = RetrieveTokenRequest();
			var tokenObject = JsonConvert.SerializeObject(tokenRequest);
			request.AddParameter("application/json; charset=utf-8", 
								 tokenObject, ParameterType.RequestBody);
			request.RequestFormat = DataFormat.Json;

			IRestResponse response = client.Execute(request);


			var tokenResult = JsonConvert
							  .DeserializeObject<Token>(response.Content);

			return new LoginResult
			{
				UserId = user.Id,
				Username = user.Username,
				Token = tokenResult.AccessToken,
				TokenType = tokenResult.TokenType,
				Expiration = tokenResult.Expiration
			};
		}
		
		private TokenRequest RetrieveTokenRequest()
		{
			return new TokenRequest
			{
				ClientId = _clientId,
				ClientSecret = _clientSecret,
				Audience = _audience,
				GrantType = _grantType
			};
		}

		private void InitializeValues()
		{
			_clientId = _config["Auth0:ClientId"];
			_clientSecret = _config["Auth0:ClientSecret"];
			_audience = _config["Auth0:ApiIdentifier"];
			_grantType = "client_credentials";
			_url = $"https://{_config["Auth0:Domain"]}";

			//PrintCredentials();
		}

		// For testing purposes
		private void PrintCredentials()
		{
			Console.WriteLine("Auth0 credentials:");
			Console.WriteLine($"Client Id: {_clientId}");
			Console.WriteLine($"Client Secret: {_clientSecret}");
			Console.WriteLine($"Audience: {_audience}");
			Console.WriteLine($"Url: {_url}");
		}
		#endregion


		#region Classes
		private class TokenRequest
		{
			[JsonProperty("client_id")]
			public string ClientId { get; set; }
			[JsonProperty("client_secret")]
			public string ClientSecret { get; set; }
			[JsonProperty("audience")]
			public string Audience { get; set; }
			[JsonProperty("grant_type")]
			public string GrantType { get; set; }
		}
		private class Token
		{
			[JsonProperty("access_token")]
			public string AccessToken { get; set; }
			[JsonProperty("expires_in")]
			public int Expiration { get; set; }
			[JsonProperty("token_type")]
			public string TokenType { get; set; }
		}
		#endregion
	}
}
