using System;
using System.Linq;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/login")]
    [ApiController]
    public class LoginController : ControllerBase
    {
        #region Fields
        private string _connectionString;
        private IConfiguration _config;
        private ILogger<LoginController> _logger;
        #endregion


        #region Properties
        #endregion


        #region Contructors
        public LoginController(IConfiguration config, ILogger<LoginController> logger)
        {
            _logger = logger;
            _config = config;
            _connectionString = _config.GetConnectionString("DefaultConnection");
        }
        #endregion


        #region HTTP endpoints
        public IActionResult Post([FromBody] User user)
        {
            var context = new UserContext(_connectionString);

            _logger.LogInformation("Starting process of validating credentials");
            
            var message = "Invalid credentials";
            var password = user.Password;

            var loginRes = new LoginResult
            {
                Username = user.Username
            };

            try
            {
                if (context.Users.FirstOrDefault(usr => usr.Username.Equals(user.Username)) != null)
                {
                    user = context.Users.FirstOrDefault(usr => usr.Username.Equals(user.Username));

                    var validatePass = new PasswordEncryption();
                    var validated = validatePass.VerifyPassword(user, password);
                    if (!validated)
                    {
                        loginRes.Message = message;
                        _logger.LogInformation(message);

                        return Ok(loginRes);
                    }

                    _logger.LogInformation("Successfully validated user credentials");

                    TokenManager tk = new TokenManager(_config);

                    loginRes = tk.LoginSymmetric(user);

                    return Ok(loginRes);
                }
                else
                {
                    loginRes.Message = message;

                    return NotFound(loginRes);
                }
            }
            catch (Exception ex)
            {
                _logger.LogError("An error occurred: {0}", ex.Message);
                _logger.LogError("Inner Exception: {0}", ex.InnerException.Message);
            }

            return NotFound(loginRes);
        }
        #endregion
    }
}
