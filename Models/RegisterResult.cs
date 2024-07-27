using Newtonsoft.Json;

namespace Icarus.Models;

public class RegisterResult : BaseResult
{
    #region Properties
    [JsonProperty("username")]
    public string? Username { get; set; }
    [JsonProperty("successfully_registered")]
    public bool SuccessfullyRegistered { get; set; }
    #endregion
}
