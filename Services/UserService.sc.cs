// using JwtAuthentication.Shared.Exceptions;
// using JwtAuthentication.Shared.Models;
using System.Linq;

using Microsoft.Extensions.Configuration;

using Icarus.Exceptions;
using Icarus.Models.Shared;
using Icarus.Services;
using Icarus.Repositories;
using Icarus.Database.Contexts;

namespace Icarus.Services
{
    public class UserService
    {
        private Microsoft.Extensions.Configuration.IConfiguration _configuration;
        private readonly UserRepository userRepository;
        private readonly UserContext _userContext;

        public UserService(UserRepository userRepository, Microsoft.Extensions.Configuration.IConfiguration configuration)
        {
            this._configuration = configuration;
            this.userRepository = userRepository;
        }

        // public UserService(UserContext userContext, Microsoft.Extensions.Configuration.IConfiguration configuration)
        public UserService(Microsoft.Extensions.Configuration.IConfiguration configuration)
        {
            this._configuration = configuration;
            this._userContext = new UserContext(configuration.GetConnectionString("DefaultConnection"));
        }

        public void ValidateCredentials(UserCredentials userCredentials)
        {
            // Icarus.Models.User user = userRepository.GetUser(userCredentials.Username);
            var user = _userContext.Users.FirstOrDefault(usr => usr.Username.Equals(userCredentials.Username));
            bool isValid = user != null && AreValidCredentials(userCredentials, user);

            if (!isValid)
            {
                throw new InvalidCredentialsException();
            }
        }

        private static bool AreValidCredentials(UserCredentials userCredentials, Icarus.Models.User user)
        {
            // TODO: Has the user provided password and compair it to the hash retrieved from
            // the DB
            return true;
        }
    }
}