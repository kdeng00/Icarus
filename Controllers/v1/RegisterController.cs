using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/register")]
    [ApiController]
    public class RegisterController : ControllerBase
    {
        #region Fields
        private IConfiguration _config;
        #endregion


        #region Properties
        #endregion


        #region Constructor
        public RegisterController(IConfiguration config)
        {
            _config = config;
        }
        #endregion

        [HttpPost]
        public IActionResult Post([FromBody] User user)
        {
            PasswordEncryption pe = new PasswordEncryption();
            user.Password = pe.HashPassword(user);
            user.EmailVerified = false;

            var context = new UserContext(_config.GetConnectionString("DefaultConnection"));

            try
            {
                context.Add(user);
                context.SaveChanges();
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                var stackTrace = ex.StackTrace;

                Console.WriteLine($"An error occurred: {msg}");
            }

            var registerResult = new RegisterResult
            {
                Username = user.Username
            };

            if (context.Users.FirstOrDefault(sng => sng.Username.Equals(user.Username)) != null)
            {
                registerResult.Message = "Successful registration";
                registerResult.SuccessfullyRegistered = true;

                return Ok(registerResult);
            }
            else
            {
                registerResult.Message = "Registration failed";
                registerResult.SuccessfullyRegistered = false;

                return Ok(registerResult);
            }
        }
    }
}
