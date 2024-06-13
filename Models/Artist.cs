using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models;

public class Artist
{
    [JsonProperty("artist_id")]
    public int ArtistID { get; set; }
    [JsonProperty("name")]
    [Column("Artist")]
    public string Name { get; set; }
    [JsonProperty("song_count")]
    [NotMapped]
    public int SongCount { get; set; }
}
