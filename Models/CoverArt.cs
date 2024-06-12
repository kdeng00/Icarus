using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations.Schema;
using System.Linq;
using System.Text;

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
        const int length = 25;
        const string chars = "ABCDEF0123456789";
        var random = new Random();
        var filename = new string(Enumerable.Repeat(chars, length).Select(s =>
            s[random.Next(s.Length)]).ToArray());
        var extension = Icarus.Constants.FileExtensions.JPG_EXTENSION;

        return (flag == 0) ? filename : $"{filename}{extension}";
    }
    #endregion
}
