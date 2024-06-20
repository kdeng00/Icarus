using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models;

public class Song
{
    #region Properties
    [JsonProperty("id")]
    public int Id { get; set; }
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
    public int? AlbumId { get; set; }
    [JsonIgnore]
    public int? ArtistId { get; set; }
    [JsonIgnore]
    public int? GenreId { get; set; }
    [JsonIgnore]
    public int? CoverArtId { get; set; }
    [JsonProperty("date_created")]
    public DateTime DateCreated { get; set; }
    [JsonProperty("user_id")]
    public int UserId { get; set; }
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
        int length = Constants.DirectoryPaths.FILENAME_LENGTH;
        string chars = Constants.DirectoryPaths.FILENAME_CHARACTERS;
        var filename = this.Generate(length, chars);
        var extension = Icarus.Constants.FileExtensions.WAV_EXTENSION;

        return flag == 0 ? filename : $"{filename}{extension}";
    }
    public async Task<string> GenerateFilenameAsync(int flag = 0)
    {
        int length = Constants.DirectoryPaths.FILENAME_LENGTH;
        string chars = Constants.DirectoryPaths.FILENAME_CHARACTERS;
        var extension = Icarus.Constants.FileExtensions.WAV_EXTENSION;
        var filename = await Task.Run(() =>
        {
            return this.Generate(length, chars);
        });

        return flag == 0 ? filename : $"{filename}{extension}";
    }

    private string Generate(int length, string chars)
    {
        var random = new Random();
        var filename = new string(Enumerable.Repeat(chars, length).Select(s =>
            s[random.Next(s.Length)]).ToArray());
        return filename;
    }
    #endregion
}
