using System;
using System.Text;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class CoverArt
    {
        [JsonProperty("id")]
        public int Id { get; set; }
        [JsonProperty("title")]
	public string SongTitle { get; set; }
        [JsonIgnore]
	public string ImagePath { get; set; }
    }
}
