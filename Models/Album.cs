using System;

using Newtonsoft.Json;

namespace Icarus.Models
{
	public class Album
	{
		[JsonProperty("id")]
		public int Id { get; set; }
		[JsonProperty("title")]
		public string Title { get; set; }
		[JsonProperty("album_artist")]
		public string AlbumArtist { get; set; }
		[JsonProperty("song_count")]
		public int SongCount { get; set; }
	}
}
