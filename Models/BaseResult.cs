using System;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class BaseResult
    {
        [JsonProperty("message")]
        public string Message { get; set; }
    }
}
