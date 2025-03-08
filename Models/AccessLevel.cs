using System.ComponentModel.DataAnnotations.Schema;

namespace Icarus.Models;

public class AccessLevel
{
    #region Properties
    [Newtonsoft.Json.JsonProperty("id")]
    public int Id { get; set; }
    [Newtonsoft.Json.JsonProperty("level")]
    public string? Level { get; set; }
    [Newtonsoft.Json.JsonProperty("song_id")]
    public int SongId { get; set; }
    #endregion
}