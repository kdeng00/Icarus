using System;
using System.IO;
using System.Linq;

using Microsoft.Extensions.Configuration;

using Icarus.Models;

namespace Icarus.Controllers.Managers
{
    public class DirectoryManager : BaseManager
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
        public DirectoryManager(IConfiguration config)
        {
            _config = config;
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

            var songPath = string.Empty;
            var artistPath = ArtistDirectory(song);
            var albumPath = AlbumDirectory(song);

            if (!Directory.Exists(artistPath))
            {
                _logger.Info("Artist path does not exist");

                Directory.CreateDirectory(artistPath);

                _logger.Info("Creating artist path");
            }
            if (!Directory.Exists(albumPath))
            {
                _logger.Info("Album path does not exist");

                Directory.CreateDirectory(albumPath);

                _logger.Info("Created album path");
            }

            songPath = albumPath;

            return songPath;
        }

        private void Initialize()
        {
            _rootSongDirectory = _config.GetValue<string>("RootMusicPath");
        }

        private bool IsDirectoryEmpty(string path)
        {
            return !Directory.EnumerateFileSystemEntries(path).Any();
        }

        private string AlbumDirectory()
        {
            string directory = _rootSongDirectory;
            directory += $@"{_song.Artist}/{_song.AlbumTitle}/";

            return directory;
        }
        private string AlbumDirectory(Song song)
        {
            var directory = _rootSongDirectory;
            directory += $@"{song.Artist}/{song.AlbumTitle}/";
            Console.WriteLine($"Album directory {directory}");

            return directory;
        }
        private string ArtistDirectory()
        {
            var directory = _rootSongDirectory;
            directory += $@"{_song.Artist}/";

            return directory;
        }
        private string ArtistDirectory(Song song)
        {
            var directory = _rootSongDirectory;
            directory += $@"{song.Artist}/";
            Console.WriteLine($"Artist directory {directory}");

            return directory;
        }
        #endregion
    }
}
