using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Logging;

using Icarus.Controllers.Managers;
using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Repositories;

namespace Icarus.Controllers.V1
{
    [Route("api/v1/login")]
    [ApiController]
    public class LoginController : ControllerBase
    {
        #region Fields
        private IConfiguration _config;
        private ILogger<LoginController> _logger;
        #endregion


        #region Properties
        #endregion


        #region Contructors
        public LoginController(IConfiguration config, ILogger<LoginController> logger)
        {
            _config = config;
            _logger = logger;
        }
        #endregion


        #region HTTP endpoints
        public IActionResult Post([FromBody] User user)
        {
            UserRepository context = HttpContext.RequestServices
                .GetService(typeof(UserRepository)) as UserRepository;

            _logger.LogInformation("Starting process of validating credentials");
            
            var loginRes = new LoginResult
            {
                Username = user.Username,
                Message = "Invalid credentials"
            };

            if (context.DoesUserExist(user))
            {
                var password = user.Password;
                user = context.RetrieveUser(user);

                var validatePass = new PasswordEncryption();
                var validated = validatePass.VerifyPassword(user, password);
                if (!validated)
                {
                    _logger.LogInformation(loginRes.Message);

                    return Ok(loginRes);
                }

                _logger.LogInformation("Successfully validated user credentials");

                var tok = new TokenReq
                {
                    ClientId = _config["Auth0:ClientId"],
                    ClientSecret = _config["Auth0:ClientSecret"],
                    Audience = _config["Auth0:ApiIdentifier"],
                    GrantType = "client_credentials",
                    URI = $"https://{_config["Auth0:Domain"]}",
                    Endpoint = "oauth/token"
                };
                
                IntPtr logRes = TokenManager.retrieve_token(ref tok);
                var lr = (LogRes)Marshal.PtrToStructure(logRes, typeof(LogRes));
                loginRes = TokenManager.ConvertLogResToLoginResult(ref lr);
                loginRes.Username = user.Username;
                loginRes.UserId = user.Id;

                return Ok(loginRes);
            }
            else
                return NotFound(loginRes);
        }
        #endregion
    }
}
