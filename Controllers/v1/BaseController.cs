using System;
using System.Linq;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;


namespace Icarus.Controllers.V1
{
    public class BaseController : ControllerBase
    {
        #region Fiends
        protected IConfiguration _config;
        #endregion


        #region Methods
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

        protected bool IsTokenValid(string scope)
        {
            var token = ParseBearerTokenFromHeader();
            var tokMgr = new TokenManager(_config);

            return tokMgr.IsTokenValid(scope, token);
        }
        #endregion
    }
}