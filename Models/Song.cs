using System;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

using Icarus.Models.Context;

namespace Icarus.Models
{
	public class Song
	{
		[JsonProperty("id")]
		public int Id { get; set; }
		[JsonProperty("title")]
		public string Title { get; set; }
		[JsonProperty("album")]
		public string AlbumTitle { get; set; }
		[JsonProperty("artist")]
		public string Artist { get; set; }
		[JsonProperty("year")]
		public int? Year { get; set; }
		[JsonProperty("genre")]
		public string Genre { get; set; }
		[JsonProperty("duration")]
		public int Duration { get; set; }
		[JsonProperty("filename")]
		public string Filename { get; set; }
		[JsonProperty("song_path")]
		public string SongPath { get; set; }

		[JsonIgnore]
		public Album Album { get; set; }
		[JsonIgnore]
		public int AlbumId { get; set; }

		[JsonIgnore]
		public Artist SongArtist { get; set; }
		[JsonIgnore]
		public int ArtistId { get; set; }

		[JsonIgnore]
		public Genre SongGenre { get; set; }
		[JsonIgnore]
		public int GenreId { get; set; }

		[JsonIgnore]
		public Year SongYear { get; set; }
		[JsonIgnore]
		public int YearId { get; set; }
	}
}
