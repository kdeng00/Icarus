using System;
using System.Collections.Generic;

using Newtonsoft.Json;

namespace Icarus.Models
{
	public class Genre
	{
		[JsonProperty("id")]
		public int GenreId { get; set; }
		[JsonProperty("genre")]
		public string GenreName { get; set; }
		[JsonProperty("song_count")]
		public int SongCount { get; set; }

		[JsonIgnore]
		public List<Song> Songs { get; set; }
	}
}
