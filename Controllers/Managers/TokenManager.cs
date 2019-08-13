using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

using Microsoft.Extensions.Configuration;
using Newtonsoft.Json;
using RestSharp;

using Icarus.Models;

namespace Icarus.Controllers.Managers
{
    public class TokenManager : BaseManager
    {
        #region Fields
        private IConfiguration _config;
        #endregion


        #region Properties
        #endregion


        #region Constructors
        public TokenManager(IConfiguration config)
        {
            _config = config;
        }
        #endregion


        #region Methods
        #region c++ Libs
        [DllImport("libicarus.so")]
        public static extern IntPtr retrieve_token(ref TokenReq req);
        #endregion

        public static LoginResult ConvertLogResToLoginResult(ref LogRes res)
        {
            return new LoginResult
            {
                Token = res.Token,
                TokenType = res.TokenType,
                Expiration = res.Expiration,
                Message = res.Message
            };
        }
        #endregion
    }
}
