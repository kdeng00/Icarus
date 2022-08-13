// using JwtAuthentication.Shared.Exceptions;
// using JwtAuthentication.Shared.Models;
using Icarus.Exceptions;
using Icarus.Models.Shared;
using Icarus.Services;
using Icarus.Repositories;

namespace Icarus.Services
{
    public class UserService
    {
        private Microsoft.Extensions.Configuration.IConfiguration _configuration;
        private readonly UserRepository userRepository;

        public UserService(UserRepository userRepository, Microsoft.Extensions.Configuration.IConfiguration configuration)
        {
            this._configuration = configuration;
            this.userRepository = userRepository;
        }

        public void ValidateCredentials(UserCredentials userCredentials)
        {
            Icarus.Models.User user = userRepository.GetUser(userCredentials.Username);
            bool isValid = user != null && AreValidCredentials(userCredentials, user);

            if (!isValid)
            {
                throw new InvalidCredentialsException();
            }
        }

        private static bool AreValidCredentials(UserCredentials userCredentials, Icarus.Models.User user)
        {
            return user.Username == userCredentials.Username &&
                   user.Password == userCredentials.Password;
        }
    }
}