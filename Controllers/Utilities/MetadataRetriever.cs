using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using Microsoft.Extensions.Logging;
using NLog;
using TagLib;

using Icarus.Models;
using Icarus.Controllers.Managers;

namespace Icarus.Controllers.Utilities
{
    public class MetadataRetriever
    {
        #region Fields
        private static NLog.Logger _logger = NLog.Web.NLogBuilder.ConfigureNLog("nlog.config").GetCurrentClassLogger();
        private string _message;
        #endregion


        #region Properties
        public string Message
        {
            get => _message;
            set => _message = value;
        }
        #endregion


        #region Methods
        public static void PrintMetadata(Song song)
        {
            Console.WriteLine("\n\nMetadata of the song:");
            Console.WriteLine($"Id: {song.Id}");
            Console.WriteLine($"Title: {song.Title}");
            Console.WriteLine($"Artist: {song.Artist}");
            Console.WriteLine($"Album: {song.AlbumTitle}");
            Console.WriteLine($"Genre: {song.Genre}");
            Console.WriteLine($"Year: {song.Year}");
            Console.WriteLine($"Duration: {song.Duration}");
            Console.WriteLine($"AlbumId: {song.AlbumId}");
            Console.WriteLine($"ArtistId: {song.ArtistId}");
            Console.WriteLine($"GenreId: {song.GenreId}");
            Console.WriteLine($"YearId: {song.YearId}");
            Console.WriteLine($"Song Path: {song.SongPath}");
            Console.WriteLine($"Filename: {song.Filename}");
            Console.WriteLine("\n");

            _logger.Info("Metadata of the song");
            _logger.Info($"Title: {song.Title}");
            _logger.Info($"Artist: {song.Artist}");
            _logger.Info($"Album: {song.AlbumTitle}");
            _logger.Info($"Genre: {song.Genre}");
            _logger.Info($"Year: {song.Year}");
            _logger.Info($"Duration: {song.Duration}");
        }

        #region C++ Libs
        [DllImport("libicarus.so")]
        public static extern void retrieve_metadata(ref Sng sng, string file_path);

        [DllImport("libicarus.so")]
        public static extern void update_metadata(ref Sng sng_updated, ref Sng sng_old);

        [DllImport("libicarus.so")]
        public static extern void update_cover_art(ref CovArt cov, string stock_cover_path, string song_path);
        #endregion

        // TODO: Remove this when the update_cover_art(..) has been implemented
        /**
        public byte[] RetrieveCoverArtBytes(Song song)
        {
            try
            {
                Console.WriteLine("Fetching image");
                var tag = TagLib.File.Create(song.SongPath);
                byte[] imgBytes = tag.Tag.Pictures[0].Data.Data;
            
                return imgBytes;
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred in MetadataRetriever");
            }

            return null;
        }
        */

        // TODO: Remove this when the update_cover_art(..) has been implemented
        public void UpdateCoverArt(Song song, CoverArt coverArt)
        {
            Console.WriteLine("Updating song's cover art");

            var tag = TagLib.File.Create(song.SongPath);
            var pics = tag.Tag.Pictures;
            Array.Resize(ref pics, 1);

            pics[0] = new Picture(coverArt.ImagePath)
            {
                Description = "Cover Art"
            };

            tag.Tag.Pictures = pics;
            tag.Save();
        }
        #endregion
    }
}
