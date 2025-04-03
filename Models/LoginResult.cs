using Newtonsoft.Json;

namespace Icarus.Models;

public class LoginResult : BaseResult
{
    #region Properties
    [JsonProperty("user_id")]
    public Guid UserId { get; set; }
    [JsonProperty("username")]
    public string? Username { get; set; }
    [JsonProperty("token")]
    public string? Token { get; set; }
    [JsonProperty("token_type")]
    public string? TokenType { get; set; }
    [JsonProperty("expiration")]
    public int Expiration { get; set; }
    #endregion
}
