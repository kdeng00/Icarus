using Microsoft.AspNetCore.Mvc;


namespace Icarus.Controllers.V1;

public class BaseController : ControllerBase
{
    #region Fiends
    protected IConfiguration? _config;
    #endregion


    #region Methods
    [ApiExplorerSettings(IgnoreApi = true)]
    [Obsolete("Asymmetric key signing for tokens have been deprecated")]
    protected string ParseBearerTokenFromHeader()
    {
        var token = string.Empty;
        const string tokenType = "Bearer";
        const string otherTokenType = "Jwt";

        var req = Request;
        var auth = req.Headers.Authorization;
        var val = auth.ToString();

        if ((val.Contains(tokenType) || val.Contains(otherTokenType)) && val.Split(" ").Count() > 1)
        {
            var split = val.Split(" ");
            token = split[1];
        }


        return token;
    }

    #endregion
}
