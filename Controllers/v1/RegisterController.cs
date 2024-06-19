using Microsoft.AspNetCore.Mvc;

using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.V1;

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
    public IActionResult RegisterUser([FromBody] User user)
    {
        PasswordEncryption pe = new PasswordEncryption();
        user.Password = pe.HashPassword(user);
        user.EmailVerified = false;
        user.Status = "Registered";
        user.DateCreated = DateTime.Now;

        UserContext context = null;

        try
        {
            context = new UserContext(_config.GetConnectionString("DefaultConnection"));
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
