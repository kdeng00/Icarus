using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;
using System.Text;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class CoverArt
    {
        [JsonProperty("cover_art_id")]
        public int CoverArtID { get; set; }
        [JsonProperty("title")]
        public string SongTitle { get; set; }
        [JsonIgnore]
        public string ImagePath { get; set; }
        [JsonProperty("song_id")]
        public int SongId { get; set; }
        [JsonIgnore]
        [NotMapped]
        public List<Song> Songs { get; set; }
    }
}
