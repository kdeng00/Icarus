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
        #region Methods
        #region C++ libs
        [DllImport("libicarus.so")]
        public static extern void create_directory(Sng song, string root_path, StringBuilder created_dir);

        [DllImport("libicarus.so")]
        public static extern void copy_stock_cover_art(string target_path, string source_path);

        [DllImport("libicarus.so")]
        public static extern void copy_song(string target_path, string source_path);

        [DllImport("libicarus.so")]
        public static extern void delete_cover_art(string cover_art_path, string stock_path);

        [DllImport("libicarus.so")]
        public static extern void delete_empty_directories(Sng song, string root_path);

        [DllImport("libicarus.so")]
        public static extern void delete_from_filesystem(Sng song);

        [DllImport("libicarus.so")]
        public static extern void delete_song_empty_directories(Sng song, string root_path);
        #endregion
        #endregion
    }
}
