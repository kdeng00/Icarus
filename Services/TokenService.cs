// using JwtAuthentication.AsymmetricEncryption.Certificates;
using System;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Linq;

using Microsoft.IdentityModel.Tokens;
using Microsoft.Extensions.Configuration;

using Icarus;
using Icarus.Certs;
using Icarus.Database.Contexts;
using Icarus.Repositories;
using Icarus.Models.Shared;
// using JwtAuthentication.Shared;
// using JwtAuthentication.Shared.Models;

namespace Icarus.Services
{
    public class TokenService
    {
        private Microsoft.Extensions.Configuration.IConfiguration _configuration;
        private readonly UserRepository userRepository;
        private readonly UserContext _userContext;
        private readonly Icarus.Certs.SigningAudienceCertificate signingAudienceCertificate;
        private string _publickKeyPath = string.Empty;

        /**
        public TokenService(UserRepository userRepository)
        {
            this.userRepository = userRepository;
            signingAudienceCertificate = new SigningAudienceCertificate();
        }
        */

        /**
        public TokenService(UserRepository userRepository, Microsoft.Extensions.Configuration.IConfiguration configuration)
        {
            this._configuration = configuration;
            this.userRepository = userRepository;
            signingAudienceCertificate = new Icarus.Certs.SigningAudienceCertificate();
            _publickKeyPath = configuration["RSAKeys:PublicKeyPath"];
        }
        */

        public TokenService(Microsoft.Extensions.Configuration.IConfiguration configuration)
        {
            this._configuration = configuration;
            // this.userRepository = userRepository;
            this._userContext = new UserContext(configuration.GetConnectionString("DefaultConnection"));
            signingAudienceCertificate = new Icarus.Certs.SigningAudienceCertificate();
            _publickKeyPath = configuration["RSAKeys:PublicKeyPath"];
        }

        public string GetToken(string username)
        {
            // var user = userRepository.GetUser(username);
            var user = _userContext.Users.FirstOrDefault(usr => usr.Username.Equals(username));
            SecurityTokenDescriptor tokenDescriptor = GetTokenDescriptor(user);

            var tokenHandler = new JwtSecurityTokenHandler();
            SecurityToken securityToken = tokenHandler.CreateToken(tokenDescriptor);
            string token = tokenHandler.WriteToken(securityToken);

            return token;
        }

        private SecurityTokenDescriptor GetTokenDescriptor(Icarus.Models.User user)
        {
            const int expiringDays = 7;

            var tokenDescriptor = new SecurityTokenDescriptor
            {
                Subject = new ClaimsIdentity(user.Claims()),
                Expires = DateTime.UtcNow.AddDays(expiringDays),
                SigningCredentials = signingAudienceCertificate.GetAudienceSigningKey(_publickKeyPath)
            };

            return tokenDescriptor;
        }
    }
}