using Icarus.Models;
using Icarus.Types;

namespace Icarus.Controllers.Managers;

// NOTE: Do not use metadata for the song's metadata
public class DirectoryManager : BaseManager
{
    #region Fields
    private Song? _song;
    private string? _rootSongDirectory;
    private string? _songDirectory;
    #endregion


    #region Properties
    public string? SongDirectory
    {
        get => _songDirectory;
        set => _songDirectory = value;
    }
    #endregion


    #region Constructors
    public DirectoryManager(IConfiguration config, Song song)
    {
        _config = config;
        _song = song;
        Initialize();
    }
    public DirectoryManager(IConfiguration config)
    {
        _config = config;
        Initialize();
    }
    public DirectoryManager(string rootDirectory)
    {
        _rootSongDirectory = rootDirectory;
    }
    #endregion


    #region Methods
    // Does not include extension
    public static string GenerateFilename(int length)
    {
        string chars = Constants.DirectoryPaths.FILENAME_CHARACTERS;
        var random = new Random();
        return new string(Enumerable.Repeat(chars, length).Select(s =>
            s[random.Next(s.Length)]).ToArray());
    }

    public static string GenerateDownloadFilename(int length, string extension, string title, bool? randomize)
    {
        if (randomize.HasValue && randomize.Value)
        {
            return GenerateFilename(length) + extension;
        }
        else
        {
            return title + extension;
        }
    }

    public void CreateDirectory()
    {
        this.CreateDirectory(_song!);
    }

    public void CreateDirectory(Song song)
    {
        this._song = song;

        try
        {
            _songDirectory = AlbumDirectory();

            if (!Directory.Exists(_songDirectory))
            {
                Directory.CreateDirectory(_songDirectory);
                Console.WriteLine($"The directory has been created");
            }

            Console.WriteLine($"The song will be saved in the following" +
                    $" directory: {_songDirectory}");
        }
        catch (Exception ex)
        {
            var msg = ex.Message;
            _logger.Error(msg, "An error occurred");
        }
    }


    public int DeleteEmptyDirectories(string? directory, int level)
    {
        var deleted = 0;

        try
        {
            var curDir = directory;
            for (var i = 0; i < level; i++)
            {
                if (!System.IO.Directory.Exists(curDir))
                {
                    continue;
                }

                if (this.IsDirectoryEmpty(curDir))
                {
                    System.IO.Directory.Delete(curDir);
                    deleted++;
                }

                var parentDirectory = System.IO.Directory.GetParent(curDir);

                curDir = parentDirectory!.ToString();
            }
        }
        catch (Exception ex)
        {
            var exMsg = ex.Message;
            Console.WriteLine($"An error occurred {exMsg}");
        }

        return deleted;
    }

    public void DeleteEmptyDirectories(Song song)
    {
        try
        {
            var albumDirectory = AlbumDirectory(song);
            var artistDirectory = ArtistDirectory(song);

            if (Directory.Exists(albumDirectory) && IsDirectoryEmpty(albumDirectory))
            {
                Directory.Delete(albumDirectory);
                _logger.Info("Album directory deleted");
            }
            if (Directory.Exists(artistDirectory) && IsDirectoryEmpty(artistDirectory))
            {
                Directory.Delete(artistDirectory);
                _logger.Info("Artist directory deleted");
            }
        }
        catch (Exception ex)
        {
            var msg = ex.Message;
            _logger.Error(msg, "An error occurred");
        }
    }



    private void Initialize(DirectoryType dirTypes = DirectoryType.Music)
    {
        switch (dirTypes)
        {
            case DirectoryType.Music:
                _rootSongDirectory = _config!.GetValue<string>("RootMusicPath")!;
                break;
            case DirectoryType.CoverArt:
                _rootSongDirectory = _config!.GetValue<string>("CoverArtPath")!;
                break;
        }
    }

    private bool IsDirectoryEmpty(string path) =>
        !(Directory.EnumerateFileSystemEntries(path).Any());

    private string AlbumDirectory()
    {
        return AlbumDirectory(_song!);
    }
    private string AlbumDirectory(Song song)
    {
        var directory = ArtistDirectory(song);
        var segment = SerializeValue(song.AlbumTitle!);
        directory += $@"{segment}/";
        Console.WriteLine($"Album directory {directory}");

        return directory;
    }
    private string ArtistDirectory(Song song)
    {
        var directory = _rootSongDirectory;
        var segment = SerializeValue(song.Artist!);
        directory += $@"{segment}/";
        Console.WriteLine($"Artist directory {directory}");

        return directory;
    }

    private string SerializeValue(string value)
    {
        const int length = 15;
        const string chars = "ABCDEF0123456789";
        var random = new Random();
        var output = new string(Enumerable.Repeat(chars, length).Select(s =>
            s[random.Next(s.Length)]).ToArray());

        return output;
    }
    #endregion
}
