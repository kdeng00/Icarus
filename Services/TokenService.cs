// using JwtAuthentication.AsymmetricEncryption.Certificates;
using Icarus;
using Icarus.Repositories;
using Icarus.Models.Shared;
// using JwtAuthentication.Shared;
// using JwtAuthentication.Shared.Models;
using Microsoft.IdentityModel.Tokens;
using System;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;

namespace Icarus.Services
{
    public class TokenService
    {
        private Microsoft.Extensions.Configuration.IConfiguration _configuration;
        private readonly UserRepository userRepository;
        private readonly SigningAudienceCertificate signingAudienceCertificate;
        private string _publickKeyPath = string.Empty;

        /**
        public TokenService(UserRepository userRepository)
        {
            this.userRepository = userRepository;
            signingAudienceCertificate = new SigningAudienceCertificate();
        }
        */

        public TokenService(UserRepository userRepository, Microsoft.Extensions.Configuration.IConfiguration configuration)
        {
            this._configuration = configuration;
            this.userRepository = userRepository;
            signingAudienceCertificate = new SigningAudienceCertificate();
            _publickKeyPath = configuration["RSAKeys:PublicKeyPath"];
        }

        public string GetToken(string username)
        {
            var user = userRepository.GetUser(username);
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