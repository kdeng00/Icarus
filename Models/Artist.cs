using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models
{
	public class Artist
	{
		[JsonProperty("id")]
		public int ArtistId { get; set; }
		[JsonProperty("name")]
		public string Name { get; set; }
		[JsonProperty("song_count")]
		[NotMapped]
		public int SongCount { get; set; }

		[JsonIgnore]
		public List<Song> Songs { get; set; }
	}
}
