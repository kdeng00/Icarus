using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models;

public class Genre
{
    #region Properties
    [JsonProperty("id")]
    public Guid Id { get; set; }
    [JsonProperty("genre")]
    [Column("Category")]
    public string? GenreName { get; set; }
    [JsonProperty("song_count")]
    [NotMapped]
    public int SongCount { get; set; }
    #endregion
}
