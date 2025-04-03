using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models;

[Table("User")]
public class User
{
    #region Properties
    [JsonProperty("id")]
    [Column("Id")]
    [Key]
    public Guid Id { get; set; }
    [JsonProperty("username")]
    public string? Username { get; set; }
    [JsonProperty("password")]
    public string? Password { get; set; }
    [JsonProperty("email")]
    public string? Email { get; set; }
    [JsonProperty("phone")]
    [Column("Phone")]
    public string? Phone { get; set; }
    [JsonProperty("firstname")]
    public string? Firstname { get; set; }
    [JsonProperty("lastname")]
    public string? Lastname { get; set; }
    [JsonProperty("email_verified")]
    [NotMapped]
    public bool EmailVerified { get; set; }
    [JsonProperty("date_created")]
    public DateTime DateCreated { get; set; }
    [JsonProperty("status")]
    public string? Status { get; set; }
    [JsonProperty("last_login")]
    public DateTime? LastLogin { get; set; }

    [JsonIgnore]
    [NotMapped]
    public System.Collections.Generic.IEnumerable<string>? Roles { get; set; }
    #endregion


    #region Methods
    public System.Collections.Generic.IEnumerable<System.Security.Claims.Claim> Claims()
    {
        var claims = new System.Collections.Generic.List<System.Security.Claims.Claim> { new System.Security.Claims.Claim(System.Security.Claims.ClaimTypes.Name, Username!) };
        claims.AddRange(Roles!.Select(role => new System.Security.Claims.Claim(System.Security.Claims.ClaimTypes.Role, role)));

        return claims;
    }
    #endregion
}
