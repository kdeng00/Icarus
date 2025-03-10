using System.Security.Claims;
using System.IdentityModel.Tokens.Jwt;
using System.Text;

using Microsoft.IdentityModel.Tokens;
using Newtonsoft.Json;
using RestSharp;

using Icarus.Models;
using Microsoft.VisualBasic;

namespace Icarus.Controllers.Managers;

#region Classes
public class TokenManager : BaseManager
{
    #region Fields
    private string? _clientId;
    private string? _clientSecret;
    private string? _privateKeyPath = string.Empty;
    private string? _privateKey = string.Empty;
    private string? _publicKeyPath = string.Empty;
    private string? _publicKey = string.Empty;
    private string? _audience;
    private string? _grantType;
    private string? _url;
    private string? SUCCESSFUL_TOKEN_MESSAGE = "Successfully retrieved token";
    #endregion


    #region Properties
    #endregion


    #region Constructors
    public TokenManager(IConfiguration config)
    {
        _config = config;
        InitializeValues();
    }
    #endregion


    #region Methods
    public LoginResult RetrieveLoginResult(User user)
    {
        _logger.Info("Preparing Auth0 API request");

        var client = new RestClient(_url!);
        var request = new RestRequest("oauth/token", Method.POST);
        var tokenRequest = RetrieveTokenRequest();

        _logger.Info("Serializing token object into JSON");
        var tokenObject = JsonConvert.SerializeObject(tokenRequest);

        request.AddParameter("application/json; charset=utf-8",
            tokenObject, ParameterType.RequestBody);

        request.RequestFormat = DataFormat.Json;

        _logger.Info("Sending request");
        IRestResponse response = client.Execute(request);
        _logger.Info("Response received");


        _logger.Info("Deserializing response");
        var tokenResult = JsonConvert
            .DeserializeObject<TokenTierOne>(response.Content);
        _logger.Info("Response deserialized");

        return this.InitializeLoginResult(user, tokenResult!);
    }


    public LoginResult LoginSymmetric(User user)
    {
        var tokenResult = new TokenTierOne { TokenType = "JWT" };

        var payload = Payload();
        payload.Add(new Claim("user_id", user.Id.ToString(), ClaimValueTypes.Integer));

        var tokenHandler = new JwtSecurityTokenHandler();
        var key = Encoding.ASCII.GetBytes(_config!["JWT:Secret"]!);
        var tokenDescriptor = new SecurityTokenDescriptor
        {
            Subject = new ClaimsIdentity(new Claim[]
            {
                new Claim("user_id", user.Id.ToString(), ClaimValueTypes.Integer)
                // Add more claims as needed
            }),
            Expires = DateTime.UtcNow.AddHours(1),
            SigningCredentials = new SigningCredentials(new SymmetricSecurityKey(key), SecurityAlgorithms.HmacSha256Signature),
            Issuer = _config["Jwt:Issuer"], // Add this line
            Audience = _config["Jwt:Audience"]
        };

        tokenResult.AccessToken = tokenHandler.WriteToken(tokenHandler.CreateToken(tokenDescriptor));

        var expClaim = payload.FirstOrDefault(cl =>
        {
            return cl.Type.Equals("exp");
        });

        var expiredDate = DateTime.Parse(expClaim!.Value);
        var exp = Math.Floor((expiredDate - DateTime.UnixEpoch).TotalSeconds);
        tokenResult.Expiration = Convert.ToInt32(exp);

        return this.InitializeLoginResult(user, tokenResult);
    }

    private LoginResult InitializeLoginResult(User user, TokenTierOne token)
    {
        return new LoginResult
        {
            UserId = user.Id,
            Username = user.Username,
            Token = token.AccessToken,
            TokenType = token.TokenType,
            Expiration = token.Expiration,
            Message = SUCCESSFUL_TOKEN_MESSAGE
        };
    }

    // TODO: Make this call to whenver media is being accessed or modified - cover art, genre, 
    // album, song, et cetera
    public bool CanAccessSong(string token, Song song, AccessLevel accessLevel)
    {
        if (accessLevel!.Level!.Equals(Models.AccessLevel.DefaultLevel().Level)) {
            return true;
        }
        var tokenUserId = this.RetrieveUserIdFromToken(token);
        if (tokenUserId == null)
        {
            return false;
        }

        return tokenUserId.Value == song.UserId;
    }

    public int? RetrieveUserIdFromToken(string token)
    {
        if (this.ContainsBearer(token))
        {
            token = this.StripBearer(token);
        }

        var tokenHandler = new JwtSecurityTokenHandler();
        var readTok = tokenHandler.ReadJwtToken(token);
        // var userId = -1;

        foreach (var item in readTok.Payload)
        {
            if (item.Key == "user_id")
            {
                // userId = 
                return Convert.ToInt32(item.Value);
                // break;
            }
        }

        return null;
    }

    private string StripBearer(string token)
    {
        var start = 6;
        var strippedToken = token.Substring(start);

        return Strings.Trim(strippedToken);
    }

    private bool ContainsBearer(string token)
    {
        return token.Contains("Bearer");
    }


    private string AllScopes()
    {
        var allScopes = new List<String>()
        {
            "download:songs",
            "read:song_details",
            "upload:songs",
            "delete:songs",
            "read:albums",
            "read:artists",
            "update:songs",
            "stream:songs",
            "read:genre",
            "read:year",
            "download:cover_art"
        };

        var scopes = string.Empty;

        for (var i = 0; i < allScopes.Count; i++)
        {
            if (i == allScopes.Count - 1)
            {
                scopes += allScopes[i];
            }
            else
            {
                scopes += allScopes[i] + " ";
            }
        }

        return scopes;
    }

    private List<Claim> Payload()
    {
        // TODO: Remove this hard coding
        var expLimit = 30;
        var currentDate = DateTime.Now;
        var expiredDate = currentDate.AddMinutes(expLimit);
        var issuer = "http://localhost:5002";
        var audience = "http://localhost:5002";
        var subject = _config!["JWT:Subject"];

        var claim = new List<System.Security.Claims.Claim>()
        {
            new Claim("scope", AllScopes(), "string"),
            new Claim(JwtRegisteredClaimNames.Exp, expiredDate.ToString()),
            new Claim(JwtRegisteredClaimNames.Aud, audience),
            new Claim(JwtRegisteredClaimNames.Iss, issuer),
            new Claim(JwtRegisteredClaimNames.Sub, subject!),
            new Claim(JwtRegisteredClaimNames.Iat, currentDate.ToString())
        };

        return claim;
    }

    [Obsolete("Deprecated function")]
    private async Task<string> ReadKeyContent(string filepath)
    {
        return await System.IO.File.ReadAllTextAsync(filepath);
    }

    private TokenRequest RetrieveTokenRequest()
    {
        _logger.Info("Retrieving token object");

        return new TokenRequest
        {
            ClientId = _clientId,
            ClientSecret = _clientSecret,
            Audience = _audience,
            GrantType = _grantType
        };
    }

    private void InitializeValues()
    {
        _logger.Info("Analyzing Auth0 information");

        _clientId = _config!["Auth0:ClientId"];
        _clientSecret = _config["Auth0:ClientSecret"];
        _audience = _config["Auth0:ApiIdentifier"];
        _grantType = "client_credentials";
        _url = $"https://{_config["Auth0:Domain"]}";
    }

    #region Testing Methods
    // For testing purposes
    private void PrintCredentials()
    {
        Console.WriteLine("Auth0 credentials:");
        Console.WriteLine($"Client Id: {_clientId}");
        Console.WriteLine($"Client Secret: {_clientSecret}");
        Console.WriteLine($"Audience: {_audience}");
        Console.WriteLine($"Url: {_url}");
    }
    #endregion
    #endregion


    #region Classes
    private class TokenRequest
    {
        [JsonProperty("client_id")]
        public string? ClientId { get; set; }
        [JsonProperty("client_secret")]
        public string? ClientSecret { get; set; }
        [JsonProperty("audience")]
        public string? Audience { get; set; }
        [JsonProperty("grant_type")]
        public string? GrantType { get; set; }
    }

    private class TokenTierOne
    {
        [JsonProperty("access_token")]
        public string? AccessToken { get; set; }
        [JsonProperty("expires_in")]
        public int Expiration { get; set; }
        [JsonProperty("token_type")]
        public string? TokenType { get; set; }
    }
    #endregion
}
#endregion
