// using JwtAuthentication.Shared.Models;
// using JwtAuthentication.Shared.Services;
using Icarus;
using Icarus.Repositories;
using Icarus.Models.Shared;
using Icarus.Services;

namespace Icarus.Services
{
    public class AuthenticationService
    {
        private Microsoft.Extensions.Configuration.IConfiguration _configuration;
        private readonly UserService userService;
        private readonly TokenService tokenService;

        public AuthenticationService(UserService userService, TokenService tokenService, Microsoft.Extensions.Configuration.IConfiguration configuration)
        {
            this.userService = userService;
            this.tokenService = tokenService;
            this._configuration = configuration;
        }

        public string Authenticate(UserCredentials userCredentials)
        {
            userService.ValidateCredentials(userCredentials);
            string securityToken = tokenService.GetToken(userCredentials.Username);

            return securityToken;
        }
    }
}