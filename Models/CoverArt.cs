using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;
using System.Runtime.InteropServices;
using System.Text;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class CoverArt
    {
        [JsonProperty("id")]
        public int CoverArtId { get; set; }
        [JsonProperty("title")]
	    public string SongTitle { get; set; }
        [JsonIgnore]
	    public string ImagePath { get; set; }
        [NotMapped]
        [JsonProperty("song_id")]
        public int SongId { get; set; }
	    [JsonIgnore]
        public List<Song> Songs { get; set; }
    }
    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct CovArt
    {
        public int CoverArtId;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string SongTitle;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string ImagePath;
    }
}
