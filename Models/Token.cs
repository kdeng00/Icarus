using Newtonsoft.Json;


namespace Icarus.Models;

public class Token
{
    #region Properties
    [JsonProperty("scope")]
    public string? Scope { get; set; }
    [JsonProperty("exp")]
    public int Expiration { get; set; }
    [JsonProperty("aud")]
    public string? Audience { get; set; }
    [JsonProperty("iss")]
    public string? Issuer { get; set; }
    [JsonProperty("iat")]
    public int Issued { get; set; }
    #endregion

    #region Methods
    public bool TokenExpired()
    {
        var currentDate = DateTime.Now;
        var currentDateInSeconds = Math.Floor((currentDate - DateTime.UnixEpoch).TotalSeconds);
        var result = (currentDateInSeconds >= Expiration) ? true : false;

        return result;
    }

    public bool ContainsScope(string desiredScope)
    {
        var result = Scope!.Contains(desiredScope);

        return result;
    }

    public bool Erroneous()
    {
        Func<string, bool> isEmpty = a => string.IsNullOrEmpty(a);
        var result = (!isEmpty(Scope!) && !isEmpty(Audience!) && !isEmpty(Issuer!) &&
            Expiration > 0 && Issued > 0) ? false : true;

        return result;
    }
    #endregion
}
