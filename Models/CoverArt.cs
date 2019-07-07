using System;
using System.Collections.Generic;
using System.Text;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class CoverArt
    {
        [JsonProperty("id")]
        public int CoverArtId { get; set; }
        [JsonProperty("title")]
	public string SongTitle { get; set; }
        [JsonIgnore]
	public string ImagePath { get; set; }

	[JsonIgnore]
	public List<Song> Songs { get; set; }
    }
}
