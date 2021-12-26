using System;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;

using Newtonsoft.Json;

namespace Icarus.Models
{
    public class Song
    {
        #region Properties
        [JsonProperty("song_id")]
        public int SongID { get; set; }
        [JsonProperty("title")]
        public string Title { get; set; }
        [JsonProperty("album")]
        [Column("Album")]
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
        [JsonIgnore]
        public string SongDirectory { get; set; }
        [JsonProperty("track")]
        public int Track { get; set; } = 0;
        [JsonProperty("disc")]
        public int Disc { get; set; } = 0;
        [JsonIgnore]
        public int? AlbumID { get; set; }
        [JsonIgnore]
        public int? ArtistID { get; set; }
        [JsonIgnore]
        public int? GenreID { get; set; }
        [JsonIgnore]
        public int? CoverArtID { get; set; }
        [JsonProperty("date_created")]
        public DateTime DateCreated { get; set; }
        #endregion


        #region Methods
        public string SongPath()
        {
            var fullPath = SongDirectory;

            if (fullPath[fullPath.Length -1] != '/')
            {
                fullPath += "/";
            }

            fullPath += Filename;

            return fullPath;
        }

        public string GenerateFilename()
        {
            const int length = 25;
            const string chars = "ABCDEF0123456789";
            var random = new Random();
            var filename = new string(Enumerable.Repeat(chars, length).Select(s =>
                s[random.Next(s.Length)]).ToArray());
            var extension = ".mp3";

            return $"{filename}{extension}";
        }
        #endregion
    }
}
