using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class Genre
    {
        [JsonProperty("genre_id")]
		public int GenreID { get; set; }
		[JsonProperty("genre")]
		public string GenreName { get; set; }
		[JsonProperty("song_count")]
		[NotMapped]
		public int SongCount { get; set; }

		[JsonIgnore]
		[NotMapped]
		public List<Song> Songs { get; set; }
    }
}
