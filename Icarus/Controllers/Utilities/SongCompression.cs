using Icarus.Models;

namespace Icarus.Controllers.Utilities;

public class SongCompression
{
    #region Fields
    string? _compressedSongFilename;
    string? _tempDirectory;
    byte[]? _uncompressedSong;
    #endregion


    #region Propterties
    public string? CompressedSongFilename
    {
        get => _compressedSongFilename;
        set => _compressedSongFilename = value;
    }
    #endregion


    #region Constructors
    public SongCompression()
    {
    }
    public SongCompression(string tempDirectory)
    {
        _tempDirectory = tempDirectory;
    }
    public SongCompression(byte[] uncompressedSong)
    {
        _uncompressedSong = uncompressedSong;
    }
    #endregion


    #region Methods
    public async Task<SongData> RetrieveCompressedSong(Song song)
    {
        SongData songData = new SongData();
        try
        {
            var archivePath = RetrieveCompressesSongPath(song);
            Console.WriteLine($"Compressed song saved to: {archivePath}");
            
            songData.Data = await System.IO.File.ReadAllBytesAsync(archivePath);
        }
        catch(Exception ex)
        {
            var exMsg = ex.Message;
            Console.WriteLine($"An error ocurred: \n{exMsg}");
        }

        return songData;
    }

    public string RetrieveCompressesSongPath(Song songDetails)
    {
        string tmpZipFilePath = _tempDirectory + songDetails.Filename;

        try
        {
            using (var fi = new FileStream(songDetails.SongPath(), FileMode.Open))
            {
                using (var z = new Ionic.Zlib.ZlibStream(fi, Ionic.Zlib.CompressionMode.Compress))
                {
                    using (var tr = new FileStream(tmpZipFilePath, FileMode.CreateNew))
                    {
                        z.CopyTo(tr);
                    }
                }
            }

            Console.WriteLine("Successfully compressed");
        }
        catch (Exception ex)
        {
            var exMsg = ex.Message;
            Console.WriteLine("An error ocurred");
            Console.WriteLine(exMsg);
        }

        if (songDetails.Filename!.Contains(Constants.FileExtensions.WAV_EXTENSION))
        {
            _compressedSongFilename = StripExtension(songDetails.Filename);
        }

        return tmpZipFilePath;
    }

    // Method not being used
    public byte[] CompressedSong(byte[] uncompressedSong)
    {
        byte[]? compressedSong = null;
        try
        {
            Console.WriteLine("Song has been successfully compressed");
        }
        catch (Exception ex)
        {
            var exMsg = ex.Message;
            Console.WriteLine("An error ocurred:");
            Console.WriteLine(exMsg);
        }

        return compressedSong!;
    }


    string StripExtension(string filename)
    {
        Console.WriteLine($"Before: {filename}");
        int filenameLength = filename.Length;
        Console.WriteLine($"Filename length {filenameLength}");
        var endIndex = filenameLength - 1;
        var startIndex = endIndex - 3;
        Console.WriteLine($"Starting index {startIndex} and ending index {endIndex}");
        var stripped = filename.Remove(startIndex, 4);
        stripped += Constants.FileExtensions.ZIP_EXTENSION;
        Console.WriteLine($"After {stripped}");

        return stripped;
    }
    #endregion
}
