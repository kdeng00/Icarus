using System;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Threading.Tasks;

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
        [JsonProperty("album_artist")]
        public string AlbumArtist { get; set; }
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
        [JsonProperty("track_count")]
        public int TrackCount { get; set; } = 0;
        [JsonProperty("disc")]
        public int Disc { get; set; } = 0;
        [JsonProperty("disc_count")]
        public int DiscCount { get; set; } = 0;
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


        #region Constructors
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

        public string GenerateFilename(int flag = 0)
        {
            const int length = 25;
            const string chars = "ABCDEF0123456789";
            var random = new Random();
            var filename = new string(Enumerable.Repeat(chars, length).Select(s =>
                s[random.Next(s.Length)]).ToArray());
            var extension = ".mp3";

            return flag == 0 ? filename : $"{filename}{extension}";
        }
        public async Task<string> GenerateFilenameAsync(int flag = 0)
        {
            const int length = 25;
            const string chars = "ABCDEF0123456789";
            var extension = ".mp3";
            var random = new Random();
            var filename = await Task.Run(() =>
            {
                return new string(Enumerable.Repeat(chars, length).Select(s =>
                    s[random.Next(s.Length)]).ToArray());
            });
            

            return flag == 0 ? filename : $"{filename}{extension}";
        }
        #endregion
    }
}
