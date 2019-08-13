using System;
using System.Runtime.InteropServices;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class LoginResult : BaseResult
    {
        [JsonProperty("id")]
        public int UserId { get; set; }
        [JsonProperty("username")]
        public string Username { get; set; }
        [JsonProperty("token")]
        public string Token { get; set; }
        [JsonProperty("token_type")]
        public string TokenType { get; set; }
        [JsonProperty("expiration")]
        public int Expiration { get; set; }
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi), Serializable]
    public struct LogRes
    {
        public int UserId;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Username;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Token;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string TokenType;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Message;
        public int Expiration;
    }
}
