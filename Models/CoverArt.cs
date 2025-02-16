using Newtonsoft.Json;

namespace Icarus.Models;

public class CoverArt
{
    #region Properties
    [JsonProperty("id")]
    public int Id { get; set; }
    [JsonProperty("title")]
    public string? SongTitle { get; set; }
    [JsonIgnore]
    public string? Directory { get; set; }
    [JsonProperty("filename")]
    public string? Filename { get; set; }
    [JsonProperty("type")]
    public string? Type { get; set; }
    #endregion


    #region Methods
    public string ImagePath()
    {
        var fullPath = this.Directory;

        if (fullPath![fullPath.Length - 1] != '/')
        {
            fullPath += "/";
        }

        fullPath += Filename;

        return fullPath;
    }

    public string GenerateFilename(int flag)
    {
        int length = Constants.DirectoryPaths.FILENAME_LENGTH;
        string chars = Constants.DirectoryPaths.FILENAME_CHARACTERS;
        var random = new Random();
        var filename = new string(Enumerable.Repeat(chars, length).Select(s =>
            s[random.Next(s.Length)]).ToArray());
        var extension = Constants.FileExtensions.JPG_EXTENSION;

        return (flag == 0) ? filename : $"{filename}{extension}";
    }

    public async Task<byte[]> GetData() => await File.ReadAllBytesAsync(this.ImagePath());
    #endregion
}
