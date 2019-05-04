using System;

using Newtonsoft.Json;

namespace Icarus.Models
{
	public class Artist
	{
		[JsonProperty("id")]
		public int Id { get; set; }
		[JsonProperty("name")]
		public string Name { get; set; }
		[JsonProperty("song_count")]
		public int SongCount { get; set; }
	}
}
