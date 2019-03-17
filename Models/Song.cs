using System;

using Newtonsoft.Json;

namespace Icarus.Models
{
	public class Song
	{
		[JsonProperty("id")]
		public int Id { get; set; }
		[JsonProperty("title")]
		public string Title { get; set; }
		[JsonProperty("album")]
		public string Album { get; set; }
		[JsonProperty("artist")]
		public string Artist { get; set; }
		[JsonProperty("year")]
		public int Year { get; set; }
		[JsonProperty("genre")]
		public string Genre { get; set; }
		[JsonProperty("duration")]
		public int Duration { get; set; }
		[JsonProperty("song_data")]
		public byte[] SongFile { get; set; }
	}
}
