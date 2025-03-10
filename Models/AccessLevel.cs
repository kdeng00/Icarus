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

    #region Methods
    public static AccessLevel DefaultLevel()
    {
        return new AccessLevel
        {
            Level = "Public"
        };
    }

    public static AccessLevel PrivateLevel()
    {
        return new AccessLevel
        {
            Level = "Private"
        };
    }

public static bool IsAccessLevelValid(string level) {
    if (level.Equals(DefaultLevel().Level)) {
        return true;
    } 
    else if (level.Equals(PrivateLevel().Level)) {
        return true;
    } 
    else {
        return false;
    }
}
    #endregion
}


