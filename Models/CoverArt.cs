using Newtonsoft.Json;

namespace Icarus.Models;

public class CoverArt
{
    #region Properties
    [JsonProperty("cover_art_id")]
    public int CoverArtID { get; set; }
    [JsonProperty("title")]
    public string SongTitle { get; set; }
    [JsonIgnore]
    public string ImagePath { get; set; }
    #endregion


    #region Methods
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

    public async Task<byte[]> GetData() => await File.ReadAllBytesAsync(this.ImagePath);
    #endregion
}
