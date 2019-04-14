using System;
using System.IO;
using System.Linq;

using Microsoft.Extensions.Configuration;

using Icarus.Models;

namespace Icarus.Controllers.Managers
{
    public class DirectoryManager
    {
 	#region Fields
	private IConfiguration _config;
        private Song _song;
	private string _rootSongDirectory;
	private string _songDirectory;
	#endregion


	#region Properties
	public string SongDirectory
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
	#endregion


	#region Methods
	public void CreateDirectory()
	{
	    try
	    {
	        _songDirectory = AlbumDirectory();

                if (!Directory.Exists(_songDirectory))
		{
		    Directory.CreateDirectory(_songDirectory);
		    Console.WriteLine("The directory has been created");
		}


		Console.WriteLine($"The song will be saved in the following" +
				  $" directory: {_songDirectory}");
	    }
	    catch (Exception ex)
	    {
	        Console.WriteLine($"An error occurred {ex.Message}");
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

	void Initialize()
	{
	    _rootSongDirectory = _config.GetValue<string>("RootMusicPath");
	}

	bool IsDirectoryEmpty(string path)
	{
    	    return !Directory.EnumerateFileSystemEntries(path).Any();
	}

	string AlbumDirectory()
	{
	    string directory = _rootSongDirectory;
	    directory += $"{_song.Artist}//{_song.Album}//";

            return directory;
	}
	string ArtistDirectory()
	{
	    var directory = _rootSongDirectory;
	    directory += $"{_song.Artist}//";

	    return directory;
	}
	#endregion
    }
}
