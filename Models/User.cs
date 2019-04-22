using System;

using Newtonsoft.Json;

namespace Icarus.Models
{
	public class User
	{
		[JsonProperty("id")]
		public int Id { get; set; }
		[JsonProperty("username")]
		public string Username { get; set; }
		[JsonProperty("password")]
		public string Password { get; set; }
		[JsonIgnore]
		public byte[] Salt { get; set; }
		[JsonProperty("email")]
		public string Email { get; set; }
		[JsonProperty("phone_number")]
		public string PhoneNumber { get; set; }
		[JsonProperty("first_name")]
		public string Firstname { get; set; }
		[JsonProperty("last_name")]
		public string Lastname { get; set; }
		[JsonProperty("date_created")]
		public DateTime DateCreated { get; set; }
		[JsonProperty("last_login")]
		public DateTime LastLogin { get; set; }
	}
}