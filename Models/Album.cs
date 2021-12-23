using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class Album
    {
        [JsonProperty("id")]
		public int AlbumId { get; set; }
		[JsonProperty("title")]
		public string Title { get; set; }
		[JsonProperty("album_artist")]
		public string AlbumArtist { get; set; }
		[JsonProperty("song_count")]
		[NotMapped]
		public int SongCount { get; set; }

		[JsonIgnore]
		public List<Song> Songs { get; set; }
    }
}
