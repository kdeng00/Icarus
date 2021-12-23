using System;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class SongResult
    {
        [JsonProperty("message")]
        public string Message { get; set; }
        [JsonProperty("song_title")]
        public string SongTitle { get; set; }
    }
}
