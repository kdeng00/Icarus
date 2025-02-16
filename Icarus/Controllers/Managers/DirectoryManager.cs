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

    public void DeleteEmptyDirectories()
    {
        try
        {
            var albumDirectory = AlbumDirectory();
            var artistDirectory = ArtistDirectory();
            if (IsDirectoryEmpty(albumDirectory))
            {
                Directory.Delete(albumDirectory);
                Console.WriteLine($"directory {albumDirectory} deleted");
            }
            if (IsDirectoryEmpty(artistDirectory))
            {
                Directory.Delete(artistDirectory);
                Console.WriteLine($"directory {artistDirectory} deleted");
            }
        }
        catch (Exception ex)
        {
            var exMsg = ex.Message;
            Console.WriteLine($"An error occurred {exMsg}");
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

                curDir = System.IO.Directory.GetParent(curDir).ToString();
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

            if (IsDirectoryEmpty(albumDirectory))
            {
                Directory.Delete(albumDirectory);
                _logger.Info("Album directory deleted");
            }
            if (IsDirectoryEmpty(artistDirectory))
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

    public string RetrieveAlbumPath(Song song)
    {
        _logger.Info("Retrieving album song path");

        var albumPath = string.Empty;
        albumPath = AlbumDirectory(song);

        return albumPath;
    }
    public string RetrieveArtistPath(Song song)
    {
        _logger.Info("Retrieving artist path");

        var artistPath = string.Empty;
        artistPath = ArtistDirectory(song);

        return artistPath;
    }

    public string GenerateSongPath(Song song)
    {
        _logger.Info("Generating song path");

        var artistPath = ArtistDirectory(song);
        var albumPath = AlbumDirectory(song);

        GenerateDirectories(new List<DirEnt>{
            new DirEnt
            {
                Pre = "Artist path does not exist",
                Path = artistPath,
                Post = "Creating artist path"
            },
            new DirEnt
            {
                Pre = "Album path does not exist",
                Path = albumPath,
                Post = "Created album path"
            }
        });

        return albumPath;
    }

    private class DirEnt
    {
        public string? Pre { get; set; }
        public string? Path { get; set; }
        public string? Post { get; set; }
    }

    private void GenerateDirectories(List<DirEnt> dirs)
    {
        foreach (var di in dirs)
        {
            _logger.Info(di.Pre);
            Directory.CreateDirectory(di.Path!);
            _logger.Info(di.Post);
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

    private bool IsDirectoryEmpty(string path)
    {
        return !Directory.EnumerateFileSystemEntries(path).Any();
    }

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
    private string ArtistDirectory()
    {
        return ArtistDirectory(_song!);
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
