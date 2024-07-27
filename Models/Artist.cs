using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models;

public class Artist
{
    #region Properties
    [JsonProperty("id")]
    public int Id { get; set; }
    [JsonProperty("name")]
    [Column("Artist")]
    public string? Name { get; set; }
    [JsonProperty("song_count")]
    [NotMapped]
    public int SongCount { get; set; }
    #endregion
}
