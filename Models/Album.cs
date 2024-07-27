using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models;

public class Album
{
    #region Properties
    [JsonProperty("id")]
    public int Id { get; set; }
    [JsonProperty("title")]
    public string? Title { get; set; }
    [JsonProperty("album_artist")]
    [Column("Artist")]
    public string? AlbumArtist { get; set; }
    [JsonProperty("song_count")]
    [NotMapped]
    public int SongCount { get; set; }
    [JsonProperty("year")]
    public int Year { get; set; }
    #endregion
}
