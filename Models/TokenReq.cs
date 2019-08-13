using System;
using System.Runtime.InteropServices;

namespace Icarus.Models
{
    [StructLayout(LayoutKind.Sequential)]
    public struct TokenReq
    {
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string ClientId;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string ClientSecret;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Audience;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string GrantType;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string URI;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Endpoint;
    }
}
