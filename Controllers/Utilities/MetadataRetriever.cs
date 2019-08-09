using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using Microsoft.Extensions.Logging;
using NLog;
using TagLib;

using Icarus.Models;

namespace Icarus.Controllers.Utilities
{
    public class MetadataRetriever
    {
        #region Fields
        private static NLog.Logger _logger = NLog.Web.NLogBuilder.ConfigureNLog("nlog.config").GetCurrentClassLogger();
        private Song _updatedSong;
        private string _message;
        #endregion


        #region Properties
        public Song UpdatedSongRecord
        {
            get => _updatedSong;
            set => _updatedSong = value;
        }
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
        public static extern void retrieve_metadata(ref Icarus.Controllers.Managers.SongManager.Sng sng, string file_path);
        #endregion

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

        public void UpdateMetadata(Song updatedSong, Song oldSong)
        {
            try
            {
                InitializeUpdatedSong(oldSong);
                var songValues = CheckSongValues(updatedSong);
                PerformUpdate(updatedSong, songValues);
                Message = "Successfully updated metadata";
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                Console.WriteLine($"An error occurred: {msg}");
                _logger.Error(msg, "An error occurred");
                Message = "Failed to update metadata";
            }
        }
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

        private void PerformUpdate(Song updatedSong, SortedDictionary<string, bool> checkedValues)
        {
            var filePath = updatedSong.SongPath;
            var title = updatedSong.Title;
            var artist = updatedSong.Artist;
            var album = updatedSong.AlbumTitle;
            var genre = updatedSong.Genre;
            var year = updatedSong.Year;
            TagLib.File fileTag = TagLib.File.Create(filePath);
            try
            {
                Console.WriteLine($"Updating metadata of {title}");
                _logger.Info($"Updating metadata of {title}");

                foreach (var key in checkedValues.Keys)
                {
                    bool result = checkedValues[key];


                    if (!result)
                        switch (key.ToLower())
                        {
                            case "title":
                                _updatedSong.Title = title;
                                fileTag.Tag.Title = title;
                                break;
                            case "artists":
                                _updatedSong.Artist = artist;
                                fileTag.Tag.Performers = new []{artist};
                                break;
                            case "album":
                                _updatedSong.AlbumTitle = album;
                                fileTag.Tag.Album = album;
                                break;
                            case "genre":
                                _updatedSong.Genre = genre;
                                fileTag.Tag.Genres = new []{genre};
                                break;
                            case "year":
                                _updatedSong.Year = year;
                                fileTag.Tag.Year = (uint)year;
                                break;
                        }
                }

                fileTag.Save();

                Console.WriteLine("Successfully updated metadata");
                _logger.Info("Successfully updated metadata");
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                Console.WriteLine($"An error occurred:\n{msg}");
                _logger.Error(msg, "An error occurred");
            }
        }
        private void InitializeUpdatedSong(Song song)
        {
            _updatedSong = new Song
            {
                Id = song.Id,
                Title = song.Title,
                AlbumTitle = song.AlbumTitle,
                Artist = song.Artist,
                Genre = song.Genre,
                Year = song.Year,
                Duration = song.Duration,
                Filename = song.Filename,
                SongPath = song.SongPath
            };
        }

        private SortedDictionary<string, bool> CheckSongValues(Song song)
        {
            var songValues = new SortedDictionary<string, bool>();
            Console.WriteLine("Checking for null data");
            _logger.Info("Checking for null data");
            try
            {
                songValues["Title"] = String.IsNullOrEmpty(song.Title);
                songValues["Artists"] = String.IsNullOrEmpty(song.Artist);
                songValues["Album"] = String.IsNullOrEmpty(song.AlbumTitle);
                songValues["Genre"] = String.IsNullOrEmpty(song.Genre);

                if (song.Year == null)
                    songValues["Year"] = true;
                else if (song.Year==0)
                    songValues["Year"] = true;
                else
                    songValues["Year"] = false;

                Console.WriteLine("Checking for null data completed");
                _logger.Info("Checking for null data completed");
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                Console.WriteLine($"An error occurred: \n{msg}");
                _logger.Error(msg, "An error occurred");
            }

            return songValues;
        }    
        #endregion
    }
}
