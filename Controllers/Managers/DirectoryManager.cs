using System;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

using Microsoft.Extensions.Configuration;

using Icarus.Models;
using Icarus.Types;

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
        public DirectoryManager(string rootDirectory)
        {
            _rootSongDirectory = rootDirectory;
        }
        #endregion


        #region Methods
        #region C++ libs
        [DllImport("libicarus.so")]
        public static extern void create_directory(SongManager.Sng song, string root_path, StringBuilder created_dir);
        [DllImport("libicarus.so")]
        public static extern void copy_stock_cover_art(string target_path, string source_path);
        [DllImport("libicarus.so")]
        public static extern void copy_song(string target_path, string source_path);
        [DllImport("libicarus.so")]
        public static extern void delete_cover_art(string cover_art_path, string stock_path);
        [DllImport("libicarus.so")]
        public static extern void delete_empty_directories(SongManager.Sng song, string root_path);
        [DllImport("libicarus.so")]
        public static extern void delete_song_empty_directories(SongManager.Sng song, string root_path);
        #endregion

        private void Initialize(DirectoryType dirTypes = DirectoryType.Music)
        {
            switch (dirTypes)
            {
                case DirectoryType.Music:
                    _rootSongDirectory = _config.GetValue<string>("RootMusicPath");
                    break;
                case DirectoryType.CoverArt:
                    _rootSongDirectory = _config.GetValue<string>("CoverArtPath");
                    break;
            }
        }
        #endregion
    }
}
