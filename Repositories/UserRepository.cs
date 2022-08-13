// using JwtAuthentication.Shared.Models;
// using Icarus.Models.Shared;
using System.Collections.Generic;
using System.Linq;

namespace Icarus.Repositories
{
    // TODO: Replace this with the UserContext.
    public class UserRepository
    {
        private readonly IEnumerable<Icarus.Models.User> users;

        public UserRepository()
        {
            users = new List<Icarus.Models.User>
            {
                new Icarus.Models.User
                {
                    Username = "john.doe",
                    Password = "john.password",
                    Roles = new []{"User"}
                },
                new Icarus.Models.User
                {
                    Username = "jane.doe",
                    Password = "jane.password",
                    Roles = new []{"User", "Admin"}
                }
            };
        }

        public Icarus.Models.User GetUser(string username)
        {
            return users.SingleOrDefault(u => u.Username.Equals(username));
        }
    }
}