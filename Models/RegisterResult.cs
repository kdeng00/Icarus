using System;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class RegisterResult : BaseResult
    {
        [JsonProperty("username")]
	    public string Username { get; set; }
	    [JsonProperty("successfully_registered")]
	    public bool SuccessfullyRegistered { get; set; }
    }
}
