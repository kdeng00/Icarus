using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.Claims;
using System.Security.Cryptography;
using System.Threading.Tasks;

using JWT;
using JWT.Serializers;
using Microsoft.Extensions.Configuration;
using Newtonsoft.Json;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Crypto.Parameters;
using Org.BouncyCastle.OpenSsl;
using Org.BouncyCastle.Security;

using RestSharp;

using Icarus.Models;

namespace Icarus.Controllers.Managers
{
    #region Classes
    public class TokenManager : BaseManager
    {
        #region Fields
        private string _clientId;
        private string _clientSecret;
        private string _privateKeyPath;
        private string _privateKey;
        private string _publicKeyPath;
        private string _publicKey;
        private string _audience;
        private string _grantType;
        private string _url;
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

            var client = new RestClient(_url);
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

            return new LoginResult
            {
                UserID = user.UserID, Username = user.Username, Token = tokenResult.AccessToken,
                TokenType = tokenResult.TokenType, Expiration = tokenResult.Expiration,
                Message = "Successfully retrieved token"
            };
        }


        public LoginResult LogIn(User user)
        {
            var tokenResult = new TokenTierOne();
            tokenResult.TokenType = "Jwt";

            var privateKey = ReadKeyContent(_privateKeyPath).Result;
            var publicKey = ReadKeyContent(_publicKeyPath).Result;

            var payload = Payload();

            var token = CreateToken(payload, privateKey);
            tokenResult.AccessToken = token;

            var expClaim = payload.FirstOrDefault(cl =>
            {
                return cl.Type.Equals("exp");
            });

            tokenResult.Expiration = System.Convert.ToInt32(expClaim.Value);

            return new LoginResult
            {
                UserID = user.UserID, Username = user.Username, Token = tokenResult.AccessToken,
                TokenType = tokenResult.TokenType, Expiration = tokenResult.Expiration,
                Message = "Successfully retrieved token"
            };
        }

        public bool IsTokenValid(string scope, string accessToken)
        {
            var result = false;
            var token = DecodeToken(accessToken);

            if (token == null || token.Erroneous())
            {
                return result;
            }

            result = (!token.TokenExpired() && token.ContainsScope(scope)) ? true : false;

            // What would make a token valid?
            // 1. The expiration date must be before the current date
            // 2. The desired scope must be part of the scopes within the access token
            // 3. Must be able to be decoded

            return result;
        }

        public Token? DecodeToken(string accessToken)
        {
            var rsaParams = GetRSAPublic(_publicKey);
            Token tok = null;

            try
            {
                using (var rsa = new RSACryptoServiceProvider())
                {
                    rsa.ImportParameters(rsaParams);

                    IJsonSerializer serializer = new JsonNetSerializer();
                    IDateTimeProvider provider = new UtcDateTimeProvider();
                    IJwtValidator validator = new JwtValidator(serializer, provider);
                    IBase64UrlEncoder urlEncoder = new JwtBase64UrlEncoder();
                    var algorithm = new JWT.Algorithms.RS256Algorithm(rsa);
                    IJwtDecoder decoder = new JwtDecoder(serializer, validator, urlEncoder, algorithm);
                    
                    var json = decoder.Decode(accessToken);
                    tok = JsonConvert.DeserializeObject<Token>(json);
                }
            }
            catch (Exception ex)
            {
                _logger.Error("An error occurred: {0}", ex.Message);
            }


            return tok;
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
            const int expLimit = 24;
            var currentDate = DateTime.Now;
            var expiredDate = currentDate.AddHours(expLimit);
            var issued = Math.Floor((currentDate - DateTime.UnixEpoch).TotalSeconds);
            var expires = Math.Floor((expiredDate - DateTime.UnixEpoch).TotalSeconds);
            var issuer = "https://soaricarus.auth0.com";
            issuer = "http://localhost:5002";
            var audience = "https://icarus/api";
            audience = "http://localhost:5002";

            var claim = new List<System.Security.Claims.Claim>()
            {
                new System.Security.Claims.Claim("scope", AllScopes(), "string"),
                new System.Security.Claims.Claim("exp", $"{expires}", "integer"),
                new System.Security.Claims.Claim("aud", $"{audience}", "string"),
                new System.Security.Claims.Claim("iss", $"{issuer}", "string"),
                new System.Security.Claims.Claim("iat", $"{issued}", "integer")
            };

            return claim;
        }

        private string CreateToken(List<Claim> claims, string privateKey)
        {
            var token = string.Empty;

            if (string.IsNullOrEmpty(privateKey))
            {
                privateKey = ReadKeyContent(_privateKeyPath).Result;
            }

            RSAParameters rsaParams;
            using (var tr = new System.IO.StringReader(privateKey))
            {
                var pemReader = new PemReader(tr);
                var keyPair = pemReader.ReadObject() as AsymmetricCipherKeyPair;
                if (keyPair == null)
                {
                    throw new Exception("Could not read RSA private key");
                } 
                var privateRsaParams = keyPair.Private as RsaPrivateCrtKeyParameters;
                rsaParams = DotNetUtilities.ToRSAParameters(privateRsaParams);
            }

            using (RSACryptoServiceProvider rsa = new RSACryptoServiceProvider())
            {
                var rsaParamsPublic = GetRSAPublic(ReadKeyContent(_publicKeyPath).Result);
                var rsaPublic = new RSACryptoServiceProvider();

                rsa.ImportParameters(rsaParams);
                rsaPublic.ImportParameters(rsaParamsPublic);

                Dictionary<string, object> payload = new Dictionary<string, object>();

                foreach (var claim in claims)
                {
                    var type = claim.Type;
                    var val = Int32.TryParse(claim.Value, out _);

                    if (val)
                    {
                        payload.Add(type, Convert.ToInt32(claim.Value));
                    }
                    else
                    {
                        payload.Add(type, claim.Value);
                    }

                }

                var algorithm = new JWT.Algorithms.RS256Algorithm(rsaPublic, rsa);
                IJsonSerializer serializer = new JsonNetSerializer();
                IBase64UrlEncoder urlEncoder = new JwtBase64UrlEncoder();
                IJwtEncoder encoder = new JwtEncoder(algorithm, serializer, urlEncoder);

                token = encoder.Encode(payload, privateKey);
            }

            return token;
        }

        private RSAParameters GetRSAPublic(string publicKey)
        {
            using (var tr = new System.IO.StringReader(publicKey))
            {
                var pemReader = new PemReader(tr);
                var publicKeyParams = pemReader.ReadObject() as RsaKeyParameters;
                if (publicKeyParams == null)
                {
                    throw new Exception("Could not read RSA public key");
                }
                return DotNetUtilities.ToRSAParameters(publicKeyParams);
            }
        }

        private async Task<string> ReadKeyContent(string filepath)
        {
            return await System.IO.File.ReadAllTextAsync(filepath);
        }
        
        private TokenRequest RetrieveTokenRequest()
        {
            _logger.Info("Retrieving token object");

            return new TokenRequest
            {
                ClientId = _clientId, ClientSecret = _clientSecret,
                Audience = _audience, GrantType = _grantType
            };
        }

        private void InitializeValues()
        {
            _logger.Info("Analyzing Auth0 information");

            _clientId = _config["Auth0:ClientId"];
            _clientSecret = _config["Auth0:ClientSecret"];
            _audience = _config["Auth0:ApiIdentifier"];
            _grantType = "client_credentials";
            _url = $"https://{_config["Auth0:Domain"]}";
            _privateKeyPath = _config["RSAKeys:PrivateKeyPath"];
            _publicKeyPath = _config["RSAKeys:PublicKeyPath"];
            _privateKey = System.IO.File.ReadAllText(_privateKeyPath);
            _publicKey = System.IO.File.ReadAllText(_publicKeyPath);

            PrintCredentials();
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
            public string ClientId { get; set; }
            [JsonProperty("client_secret")]
            public string ClientSecret { get; set; }
            [JsonProperty("audience")]
            public string Audience { get; set; }
            [JsonProperty("grant_type")]
            public string GrantType { get; set; }
        }
        private class TokenTierOne
        {
            [JsonProperty("access_token")]
            public string AccessToken { get; set; }
            [JsonProperty("expires_in")]
            public int Expiration { get; set; }
            [JsonProperty("token_type")]
            public string TokenType { get; set; }
        }
        #endregion
    }
    #endregion
}
