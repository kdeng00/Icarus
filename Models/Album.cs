using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class Album
    {
        [JsonProperty("album_id")]
        public int AlbumID { get; set; }
        [JsonProperty("title")]
        public string Title { get; set; }
        [JsonProperty("album_artist")]
        [Column("Artist")]
        public string AlbumArtist { get; set; }
        [JsonProperty("song_count")]
        [NotMapped]
        public int SongCount { get; set; }
        [JsonProperty("year")]
        public int Year { get; set; }

        [JsonIgnore]
        [NotMapped]
        public List<Song> Songs { get; set; }
    }
}
