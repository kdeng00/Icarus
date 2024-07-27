using Newtonsoft.Json;

namespace Icarus.Models;

public class SongResult
{
    #region Properties
    [JsonProperty("message")]
    public string? Message { get; set; }
    [JsonProperty("song_title")]
    public string? SongTitle { get; set; }
    #endregion
}
