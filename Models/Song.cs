using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

using Newtonsoft.Json;

namespace Icarus.Models;

public class Song
{
    #region Properties
    [JsonProperty("id")]
    [Key]
    public Guid Id { get; set; }
    [JsonProperty("title")]
    public string? Title { get; set; }
    [JsonProperty("album")]
    [Column("Album")]
    public string? AlbumTitle { get; set; }
    [JsonProperty("artist")]
    public string? Artist { get; set; }
    [JsonProperty("album_artist")]
    public string? AlbumArtist { get; set; }
    [JsonProperty("year")]
    public int? Year { get; set; }
    [JsonProperty("genre")]
    public string? Genre { get; set; }
    [JsonProperty("duration")]
    public int Duration { get; set; }
    [JsonProperty("filename")]
    public string? Filename { get; set; }
    [JsonIgnore]
    public string? SongDirectory { get; set; }
    [JsonProperty("audio_type")]
    public string? AudioType { get; set; }
    [JsonProperty("track")]
    public int Track { get; set; } = 0;
    [JsonProperty("track_count")]
    public int TrackCount { get; set; } = 0;
    [JsonProperty("disc")]
    public int Disc { get; set; } = 0;
    [JsonProperty("disc_count")]
    public int DiscCount { get; set; } = 0;
    [JsonIgnore]
    public Guid? AlbumId { get; set; }
    [JsonIgnore]
    public Guid? ArtistId { get; set; }
    [JsonIgnore]
    public Guid? GenreId { get; set; }
    [JsonIgnore]
    public Guid? CoverArtId { get; set; }
    [JsonProperty("date_created")]
    public DateTime DateCreated { get; set; }
    [JsonProperty("user_id")]
    public Guid UserId { get; set; }
    #endregion


    #region Constructors
    #endregion


    #region Methods
    public void PrintMetadata()
    {
        Console.WriteLine("\n\nMetadata of the song:");
        Console.WriteLine($"ID: {this.Id}");
        Console.WriteLine($"Title: {this.Title}");
        Console.WriteLine($"Artist: {this.Artist}");
        Console.WriteLine($"Album: {this.AlbumTitle}");
        Console.WriteLine($"Genre: {this.Genre}");
        Console.WriteLine($"Year: {this.Year}");
        Console.WriteLine($"Duration: {this.Duration}");
        Console.WriteLine($"AlbumID: {this.AlbumId}");
        Console.WriteLine($"ArtistID: {this.ArtistId}");
        Console.WriteLine($"GenreID: {this.GenreId}");
        Console.WriteLine($"Song Path: {this.SongPath()}");
        Console.WriteLine($"Filename: {this.Filename}");
        Console.WriteLine("\n");
    }

    public string SongPath()
    {
        var fullPath = SongDirectory;

        if (fullPath![fullPath.Length - 1] != '/')
        {
            fullPath += "/";
        }

        fullPath += Filename;

        return fullPath;
    }

    public string GenerateFilename(bool includeExtension = false, AudioFileExtensionsType flag = AudioFileExtensionsType.Default)
    {
        int length = Constants.DirectoryPaths.FILENAME_LENGTH;
        string chars = Constants.DirectoryPaths.FILENAME_CHARACTERS;
        var filename = this.Generate(length, chars);
        var extension = this.DetermineFileExtension(flag);

        return includeExtension ? $"{filename}{extension}" : filename;
    }
    public async Task<string> GenerateFilenameAsync(bool includeExtension = false, AudioFileExtensionsType flag = AudioFileExtensionsType.Default)
    {
        int length = Constants.DirectoryPaths.FILENAME_LENGTH;
        string chars = Constants.DirectoryPaths.FILENAME_CHARACTERS;
        var extension = this.DetermineFileExtension(flag);
        var filename = await Task.Run(() =>
        {
            return this.Generate(length, chars);
        });

        return includeExtension ? $"{filename}{extension}" : filename;
    }

    public CreateSongResult Create(Microsoft.AspNetCore.Http.IFormFile file, string filePath, string prompt)
    {
        if (System.IO.File.Exists(filePath))
        {
            return CreateSongResult.AlreadyExists;
        }

        using (var filestream = new FileStream(filePath, FileMode.Create))
        {
            Console.WriteLine(prompt);
            file.CopyTo(filestream);

            if (System.IO.File.Exists(filePath))
            {
                return CreateSongResult.Created;
            }
        }

        return CreateSongResult.NotCreated;
    }

    private string DetermineFileExtension(AudioFileExtensionsType flag)
    {
        switch (flag)
        {
            case AudioFileExtensionsType.Default:
                return Constants.FileExtensions.DEFAULT_AUDIO_EXTENSION;
            case AudioFileExtensionsType.WAV:
                return Constants.FileExtensions.WAV_EXTENSION;
            case AudioFileExtensionsType.FLAC:
                return Constants.FileExtensions.FLAC_EXTENSION;
            default:
                return "";
        }
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

#region Enums
public enum AudioFileExtensionsType
{
    Default = 0,
    WAV = 1,
    FLAC = 2
}

public enum CreateSongResult
{
    NotCreated = 0,
    AlreadyExists = 1,
    Created = 2
}
#endregion

