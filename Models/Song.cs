using System;
using System.ComponentModel.DataAnnotations.Schema;
using System.Runtime.InteropServices;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class Song
    {
        [JsonProperty("id")]
        public int Id { get; set; }
        [JsonProperty("title")]
        public string Title { get; set; }
        [JsonProperty("album")]
        public string AlbumTitle { get; set; }
        [JsonProperty("artist")]
        public string Artist { get; set; }
        [JsonProperty("year")]
   	    public int? Year { get; set; }
        [JsonProperty("genre")]
        public string Genre { get; set; }
        [JsonProperty("duration")]
        public int Duration { get; set; }
        [JsonProperty("filename")]
        public string Filename { get; set; }
        [JsonProperty("song_path")]
        public string SongPath { get; set; }

        [JsonIgnore]
        public Album Album { get; set; }
        [JsonIgnore]
        public int? AlbumId { get; set; }

        [JsonIgnore]
        public Artist SongArtist { get; set; }
        [JsonIgnore]
        public int? ArtistId { get; set; }

        [JsonIgnore]
        public Genre SongGenre { get; set; }
        [JsonIgnore]
        public int? GenreId { get; set; }

        [JsonIgnore]
        public Year SongYear { get; set; }
        [JsonIgnore]
        public int? YearId { get; set; }

        [JsonIgnore]
        public CoverArt SongCoverArt { get; set; }
        [JsonIgnore]
        public int? CoverArtId { get; set; }
    }

    [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
    public struct Sng
    {
        public int Id;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Title;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Artist;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Album;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string Genre;
        public int Year;
        public int Duration;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
        public string SongPath;
    }
}
