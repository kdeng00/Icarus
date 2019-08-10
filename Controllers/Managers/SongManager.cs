using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;
using Icarus.Database.Repositories;

namespace Icarus.Controllers.Managers
{
    public class SongManager : BaseManager
    {
        #region Fields
        private Song _song;
        private IConfiguration _config;
        private string _tempDirectoryRoot;
        private string _archiveDirectoryRoot;
        private string _compressedSongFilename;
        private string _message;
        #endregion


        #region Properties
        public Song SongDetails
        {
            get => _song;
            set => _song = value;
        }

        public string ArchiveDirectoryRoot
        {
            get => _archiveDirectoryRoot;
            set => _archiveDirectoryRoot = value;
        }
        public string CompressedSongFilename
        {
            get => _compressedSongFilename;
            set => _compressedSongFilename = value;
        }
        public string Message
        {
            get => _message;
            set => _message = value;
        }
        #endregion


        #region Constructors
        public SongManager()
        {
        }
        public SongManager(IConfiguration config)
        {
            _config = config;
        }
        public SongManager(IConfiguration config, string tempDirectoryRoot)
        {
            _config = config;
            _tempDirectoryRoot = tempDirectoryRoot;
        }
        #endregion


        #region Methods
        public SongResult UpdateSong(Song song, SongRepository songStore, AlbumRepository albumStore,
                ArtistRepository artistStore, GenreRepository genreStore, YearRepository yearStore)
        {
            var result = new SongResult();

            try
            {
                var oldSongRecord = songStore.GetSong(song);
                song.SongPath = oldSongRecord.SongPath;
                var oldSng = ConvertSongToSng(oldSongRecord);
                var updatedSng = ConvertSongToSng(song);

                MetadataRetriever.update_metadata(ref updatedSng, ref oldSng);

                var updatedSong = ConvertSngToSong(updatedSng);

                var updatedAlbum = UpdateAlbumInDatabase(oldSongRecord, updatedSong, albumStore);
                oldSongRecord.AlbumId = updatedAlbum.AlbumId;

                var updatedArtist = UpdateArtistInDatabase(oldSongRecord, updatedSong, artistStore);
                oldSongRecord.ArtistId = updatedArtist.ArtistId;

                var updatedGenre = UpdateGenreInDatabase(oldSongRecord, updatedSong, genreStore);
                oldSongRecord.GenreId = updatedGenre.GenreId;

                var updatedYear = UpdateYearInDatabase(oldSongRecord, updatedSong, yearStore);
                oldSongRecord.YearId = updatedYear.YearId;

                UpdateSongInDatabase(ref oldSongRecord, ref updatedSong, songStore, ref result);
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");

                result.Message = $"An error occurred: {msg}";
                result.SongTitle = song.Title;
            }

            return result;
        }


        public void DeleteSong(Song song, SongRepository songStore, 
                AlbumRepository albumStore, ArtistRepository artistStore, 
                GenreRepository genreStore, YearRepository yearStore, 
                CoverArtRepository coverStore)
        {
            try
            {
                DirectoryManager.delete_song_empty_directories(
                        ConvertSongToSng(song),
                            _config.GetValue<string>("RootMusicPath"));
                _logger.Info("Song deleted from the filesystem");

                var coverMgr = new CoverArtManager(_config.GetValue<string>(
                            "CoverArtPath"));
                var coverArt = coverStore.GetCoverArt(song);
                coverMgr.DeleteCoverArt(coverArt);

                coverMgr.DeleteCoverArtFromDatabase(coverArt, coverStore);
                DeleteSongFromDatabase(song, songStore, albumStore, artistStore,
                        genreStore, yearStore);
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
        }

        public async Task SaveSongToFileSystem(IFormFile songFile, SongRepository songStore,
                AlbumRepository albumStore, ArtistRepository artistStore,
                GenreRepository genreStore, YearRepository yearStore,
                CoverArtRepository coverArtStore)
        {
            try
            {
                _logger.Info("Starting the process of saving the song to the filesystem");

                var fileTempPath = Path.Combine(_tempDirectoryRoot, songFile.FileName);
                var song = await SaveSongTemp(songFile, fileTempPath);
                song.SongPath = fileTempPath;

                var rootPath = _config.GetValue<string>("RootMusicPath");
                var strCount = rootPath.Length + song.Artist.Length +
                    song.AlbumTitle.Length + 2;
                
                var filePathSB = new StringBuilder(strCount);

                DirectoryManager.create_directory(ConvertSongToSng(song),
                        rootPath, filePathSB);
                        
                var filePath = filePathSB.ToString().Substring(0, strCount);

                System.IO.File.Delete(fileTempPath);
                
                var songFilename = songFile.FileName;

                if (!songFilename.EndsWith(".mp3"))
                    filePath += $"{songFilename}.mp3";
                else
                    filePath += $"{songFilename}";

                _logger.Info($"Absolute song path: {filePath}");


                using (var fileStream = new FileStream(filePath, FileMode.Create))
                {
                    await (songFile.CopyToAsync(fileStream));
                    song.SongPath = filePath;

                    _logger.Info("Song successfully saved to filesystem");
                }

                var coverMgr = new CoverArtManager(_config.GetValue<string>("CoverArtPath"));
                var coverArt = coverMgr.SaveCoverArt(song);

                coverMgr.SaveCoverArtToDatabase(ref song, ref coverArt, 
                        coverArtStore);
                SaveSongToDatabase(song, songStore, albumStore, artistStore, genreStore,
                        yearStore);
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
        }

        public async Task<SongData> RetrieveSong(Song songMetaData)
        {
            SongData song = new SongData();
            try
            {
                Console.WriteLine("Fetching song from filesystem");
                song = await RetrieveSongFromFileSystem(songMetaData);
            }
            catch (Exception ex)
            {
                var exMsg = ex.Message;
                Console.WriteLine($"An error occurred: {exMsg}");
            }

            return song;
        }

        public static Sng ConvertSongToSng(Song song)
        {
            return new Sng
            {
                Id = (song.Id == 0) ? 0 : song.Id,
                Title = (string.IsNullOrEmpty(song.Title)) ? string.Empty :
                    song.Title,
                Artist = (string.IsNullOrEmpty(song.Artist)) ? string.Empty : 
                    song.Artist,
                Album = (string.IsNullOrEmpty(song.AlbumTitle)) ? string.Empty : 
                    song.AlbumTitle,
                Genre = (string.IsNullOrEmpty(song.Genre)) ? string.Empty : 
                    song.Genre,
                Year = (song.Year == null) ? 0 : song.Year.Value,
                Duration = song.Duration,
                SongPath = song.SongPath
            };
        }
        public static Song ConvertSngToSong(Sng song)
        {
            return new Song
            {
                Id = song.Id,
                Title = song.Title,
                Artist = song.Artist,
                AlbumTitle = song.Album,
                Genre = song.Genre,
                Year = (song.Year == 0) ? 0 : song.Year,
                Duration = song.Duration,
                SongPath = song.SongPath
            };
        }
        

        private async Task<SongData> RetrieveSongFromFileSystem(Song details)
        {
            byte[] uncompressedSong = await System.IO.File.ReadAllBytesAsync(details.SongPath);
            
            return new SongData
            {
                Data = uncompressedSong
            };
        }
        private async Task<Song> SaveSongTemp(IFormFile songFile, string filePath)
        {
            using (var filestream = new FileStream(filePath, FileMode.Create))
            {
                _logger.Info("Saving song to temporary directory");
                await songFile.CopyToAsync(filestream);
            }

            var sng = new Sng();
            MetadataRetriever.retrieve_metadata(ref sng, filePath);
            var song = ConvertSngToSong(sng);

            _logger.Info("Assigning song filename");
            song.Filename = songFile.FileName;

            return song;
        }

        private bool SongRecordChanged(Song currentSong, Song songUpdates)
        {
            var currentTitle = currentSong.Title;
            var currentArtist = currentSong.Artist;
            var currentAlbum = currentSong.AlbumTitle;
            var currentGenre = currentSong.Genre;
            var currentYear = currentSong.Year;

            if (!currentTitle.Equals(songUpdates.Title) || !currentArtist.Equals(songUpdates.Artist) ||
                    !currentAlbum.Equals(songUpdates.AlbumTitle) ||
                    !currentGenre.Equals(songUpdates.Genre) || currentYear != songUpdates.Year)
                return true;

            return false;
        }

        private void SaveSongToDatabase(Song song, SongRepository songStore, AlbumRepository albumStore,
                ArtistRepository artistStore, GenreRepository genreStore, YearRepository yearStore)
        {
            _logger.Info("Starting process to save the song to the database");

            SaveAlbumToDatabase(ref song, albumStore);
            SaveArtistToDatabase(ref song, artistStore);
            SaveGenreToDatabase(ref song, genreStore);
            SaveYearToDatabase(ref song, yearStore);

            var info = "Saving Song to DB";
            Console.WriteLine(info);
            _logger.Info(info);

            songStore.SaveSong(song);
        }
        private void SaveAlbumToDatabase(ref Song song, AlbumRepository albumStore)
        {
            _logger.Info("Starting process to save the album record of the song to the database");

            var album = new Album()
            {
                Title = song.AlbumTitle,
                AlbumArtist = song.Artist
            };

            if (!albumStore.DoesAlbumExist(song))
            {
                album.SongCount = 1;
                albumStore.SaveAlbum(album);
                album = albumStore.GetAlbum(song);
            }
            else
            {
                var albumRetrieved = albumStore.GetAlbum(song);
                album.AlbumId = albumRetrieved.AlbumId;
                album.SongCount = albumRetrieved.SongCount + 1;

                albumStore.UpdateAlbum(album);
            }

            song.AlbumId = album.AlbumId;
        }
        private void SaveArtistToDatabase(ref Song song, ArtistRepository artistStore)
        {
            _logger.Info("Starting process to save the artist record of the song to the database");

            var artist = new Artist
            {
                Name = song.Artist,
                SongCount = 1
            };

            if (!artistStore.DoesArtistExist(song))
            {
                artist.SongCount = 1;
                artistStore.SaveArtist(artist);
                artist = artistStore.GetArtist(song);
            }
            else
            {
                var artistRetrieved = artistStore.GetArtist(song);
                artist.ArtistId = artistRetrieved.ArtistId;
                artist.SongCount = artistRetrieved.SongCount + 1;

                artistStore.UpdateArtist(artist);
            }

            song.ArtistId = artist.ArtistId;
        }
        private void SaveGenreToDatabase(ref Song song, GenreRepository genreStore)
        {
            _logger.Info("Starting process to save the genre record of the song to the database");

            var genre = new Genre
            {
                GenreName = song.Genre,
                SongCount = 1
            };

            if (!genreStore.DoesGenreExist(song))
            {
                genreStore.SaveGenre(genre);
                genre = genreStore.GetGenre(song);
            }
            else
            {
                var genreRetrieved = genreStore.GetGenre(song);
                genre.GenreId = genreRetrieved.GenreId;
                genre.SongCount = genreRetrieved.SongCount + 1;

                genreStore.UpdateGenre(genre);
            }

            song.GenreId = genre.GenreId;
        }
        private void SaveYearToDatabase(ref Song song, YearRepository yearStore)
        {
            _logger.Info("Starting process to save the year record of the song to the database");

            var year = new Year
            {
                YearValue = song.Year.Value,
                SongCount = 1
            };

            if (!yearStore.DoesYearExist(song))
            {
                yearStore.SaveYear(year);
                year = yearStore.GetSongYear(song);
            }
            else
            {
                var yearRetrieved = yearStore.GetSongYear(song);
                year.YearId = yearRetrieved.YearId;
                year.SongCount = yearRetrieved.SongCount + 1;

                yearStore.UpdateYear(year);
            }

            song.YearId = year.YearId;
        }

        public Song SongCopy(Song song)
        {
            var updatedSongRecord = new Song
            {
                Id = song.Id,
                Title = song.Title,
                Artist = song.Artist,
                AlbumTitle = song.AlbumTitle,
                Genre = song.Genre,
                Year = song.Year,
                Duration = song.Duration,
                Filename = song.Filename,
                SongPath = song.SongPath,
                ArtistId = song.ArtistId,
                AlbumId = song.AlbumId,
                GenreId = song.GenreId,
                YearId = song.YearId
            };

            return updatedSongRecord;
        }

        private Album UpdateAlbumInDatabase(Song oldSongRecord, Song newSongRecord, AlbumRepository albumStore)
        {
            var albumRecord = albumStore.GetAlbum(oldSongRecord, true);
            var oldAlbumTitle = oldSongRecord.AlbumTitle;
            var oldAlbumArtist = oldSongRecord.Artist;
            var newAlbumTitle = newSongRecord.AlbumTitle;
            var newAlbumArtist = newSongRecord.Artist;

            var info = string.Empty;

            if (string.IsNullOrEmpty(newAlbumArtist))
                newAlbumArtist = oldAlbumArtist;
            if (string.IsNullOrEmpty(newAlbumTitle))
                newAlbumTitle = oldAlbumTitle;

            if ((string.IsNullOrEmpty(newAlbumTitle) && string.IsNullOrEmpty(newAlbumArtist) || 
                        oldAlbumTitle.Equals(newAlbumTitle) && oldAlbumArtist.Equals(newAlbumArtist)))
            {
                _logger.Info("No change to the song's album");
                return albumRecord;
            }

            info = "Change to the song's album";
            _logger.Info(info);

            if (!albumStore.DoesAlbumExist(newSongRecord))
            {
                _logger.Info("Creating new album record");

                albumStore.SaveAlbum(new Album
                {
                    Title = newAlbumTitle,
                    AlbumArtist = newAlbumArtist
                });

                newSongRecord.AlbumTitle = newAlbumTitle;

                return albumStore.GetAlbum(newSongRecord, true);
            }
            else
            {
                _logger.Info("Updating existing album record");

                var existingAlbumRecord = albumStore.GetAlbum(newSongRecord);
                existingAlbumRecord.AlbumArtist = newAlbumArtist;

                albumStore.UpdateAlbum(existingAlbumRecord);

                return existingAlbumRecord;
            }
        }
        private Artist UpdateArtistInDatabase(Song oldSongRecord, Song newSongRecord, ArtistRepository artistStore)
        {
            var oldArtistRecord = artistStore.GetArtist(oldSongRecord, true);
            var oldArtistName = oldArtistRecord.Name;
            var newArtistName = newSongRecord.Artist;

            if (string.IsNullOrEmpty(newArtistName) || oldArtistName.Equals(newArtistName))
            {
                _logger.Info("No change to the song's Artist");
                return oldArtistRecord;
            }

            _logger.Info("Change to the song's record found");

            if (oldArtistRecord.SongCount <= 1)
            {
                _logger.Info("Deleting artist record that no longer has any songs");

                artistStore.DeleteArtist(oldArtistRecord);
            }

            if (!artistStore.DoesArtistExist(newSongRecord))
            {
                _logger.Info("Creating new artist record");

                var newArtistRecord = new Artist
                {
                    Name = newArtistName
                };

                artistStore.SaveArtist(newArtistRecord);
                
                return artistStore.GetArtist(newSongRecord, true);
            }
            else
            {
                _logger.Info("Updating existing artist record");

                var existingArtistRecord = artistStore.GetArtist(newSongRecord);

                artistStore.UpdateArtist(existingArtistRecord);

                return existingArtistRecord;
            }
        }
        private Genre UpdateGenreInDatabase(Song oldSongRecord, Song newSongRecord, GenreRepository genreStore)
        {
            var oldGenreRecord = genreStore.GetGenre(oldSongRecord, true);
            var oldGenreName = oldGenreRecord.GenreName;
            var newGenreName = newSongRecord.Genre;

            if (string.IsNullOrEmpty(newGenreName) || oldGenreName.Equals(newGenreName))
            {
                _logger.Info("No change to the song's Genre");
                return oldGenreRecord;
            }

            _logger.Info("Change to the song's genre found");

            if (oldGenreRecord.SongCount <= 1)
            {
                _logger.Info("Deleting genre record");

                genreStore.DeleteGenre(oldGenreRecord);
            }

            if (!genreStore.DoesGenreExist(newSongRecord))
            {
                _logger.Info("Creating new genre record");

                var newGenreRecord = new Genre
                {
                    GenreName = newGenreName
                };

                genreStore.SaveGenre(newGenreRecord);

                return genreStore.GetGenre(newSongRecord, true);
            }
            else
            {
                _logger.Info("Updating existing genre record");

                var existingGenreRecord = genreStore.GetGenre(newSongRecord);

                genreStore.UpdateGenre(existingGenreRecord);
                return genreStore.GetGenre(existingGenreRecord);
            }
        }
        private Year UpdateYearInDatabase(Song oldSongRecord, Song newSongRecord, YearRepository yearStore)
        {
            var oldYearRecord = yearStore.GetSongYear(oldSongRecord, true);
            var oldYearValue = oldYearRecord.YearValue;
            var newYearValue = newSongRecord.Year;

            if (oldYearValue == newYearValue || newYearValue == 0 || newYearValue == null)
            {
                _logger.Info("No change to the song's Year");
                return oldYearRecord;
            }

            _logger.Info("Change to the song's year found");

            if (oldYearRecord.SongCount <= 1)
            {
                _logger.Info("Deleting year record");

                yearStore.DeleteYear(oldYearRecord);
            }

            if(!yearStore.DoesYearExist(newSongRecord))
            {
                _logger.Info("Creating new year record");

                var newYearRecord = new Year
                {
                    YearValue = newYearValue.Value
                };

                yearStore.SaveYear(newYearRecord);

                return yearStore.GetSongYear(newSongRecord, true);
            }
            else
            {
                _logger.Info("Updating existing year record");

                var existingYearRecord = yearStore.GetSongYear(newSongRecord);

                yearStore.UpdateYear(existingYearRecord);

                return existingYearRecord;
            }
        }
        private void UpdateSongInDatabase(ref Song oldSongRecord, ref Song newSongRecord, SongRepository songStore,
                ref SongResult result)
        {
            var updatedSongRecord = SongCopy(oldSongRecord);
            var artistOrAlbumChanged = false;

            if (!SongRecordChanged(oldSongRecord, newSongRecord))
            {
                _logger.Info("No change to the song record");
                return;
            }

            _logger.Info("Changes to song record found");

            if (!string.IsNullOrEmpty(newSongRecord.Title))
                updatedSongRecord.Title = newSongRecord.Title;
            if (!string.IsNullOrEmpty(newSongRecord.AlbumTitle))
            {
                updatedSongRecord.AlbumTitle = newSongRecord.AlbumTitle;
                artistOrAlbumChanged = true;
            }
            if (!string.IsNullOrEmpty(newSongRecord.Artist))
            {
                updatedSongRecord.Artist = newSongRecord.Artist;
                artistOrAlbumChanged = true;
            }
            if (!string.IsNullOrEmpty(newSongRecord.Genre))
            {
                updatedSongRecord.Genre = newSongRecord.Genre;
            }
            if (newSongRecord.Year > 0)
                updatedSongRecord.Year = newSongRecord.Year;
            else
                updatedSongRecord.Year = oldSongRecord.Year;

            _logger.Info("Applied changes to song record");

            if (artistOrAlbumChanged)
            {
                _logger.Info("Change to song's album or artist");

                var rootPath = _config.GetValue<string>("RootMusicPath");
                var strCount = rootPath.Length + updatedSongRecord.Artist.Length +
                    updatedSongRecord.AlbumTitle.Length + 2;
                var updatedPath = new StringBuilder(strCount);

                DirectoryManager.create_directory(ConvertSongToSng(updatedSongRecord), 
                            rootPath, updatedPath);

                var newSongPath = updatedPath.ToString().Substring(0, strCount) + 
                    updatedSongRecord.Filename;

                DirectoryManager.copy_song(newSongPath, updatedSongRecord.SongPath);

                updatedSongRecord.SongPath = newSongPath;
            }

            _logger.Info("Saving song metadata to the database");

            if (songStore.DoesSongExist(newSongRecord))
                songStore.UpdateSong(updatedSongRecord);
            else
                songStore.SaveSong(updatedSongRecord);

            newSongRecord = updatedSongRecord;

            result.Message = "Successfully updated song";
            result.SongTitle = updatedSongRecord.Title;
        }

        private void DeleteSongFromDatabase(Song song, SongRepository songStore, AlbumRepository albumStore,
                ArtistRepository artistStore, GenreRepository genreStore, YearRepository yearStore)
        {
            _logger.Info("Starting process to delete records related to the song from the database");

            DeleteAlbumFromDatabase(song, albumStore);
            DeleteArtistFromDatabase(song, artistStore);
            DeleteGenreFromDatabase(song, genreStore);
            DeleteYearFromDatabase(song, yearStore);

            songStore.DeleteSong(song);
        }
        private void DeleteAlbumFromDatabase(Song song, AlbumRepository albumStore)
        {
            if (!albumStore.DoesAlbumExist(song))
            {
                _logger.Info("Cannot delete the album record because it does not exist");
                return;
            }

            var album = albumStore.GetAlbum(song, true);

            if (album.SongCount <= 1)
                albumStore.DeleteAlbum(album);
        }
        private void DeleteArtistFromDatabase(Song song, ArtistRepository artistStore)
        {
            if (!artistStore.DoesArtistExist(song))
            {
                _logger.Info("Cannot delete the artist record because it does not exist");
                return;
            }

            var artist = artistStore.GetArtist(song, true);

            if (artist.SongCount <= 1)
                artistStore.DeleteArtist(artist);
        }
        private void DeleteGenreFromDatabase(Song song, GenreRepository genreStore)
        {
            if (!genreStore.DoesGenreExist(song))
            {
                _logger.Info("Cannot delete the genre record because it does not exist");
                return;
            }

            var genre = genreStore.GetGenre(song, true);

            if (genre.SongCount <= 1)
                genreStore.DeleteGenre(genre);
        }
        private void DeleteYearFromDatabase(Song song, YearRepository yearStore)
        {
            if (!yearStore.DoesYearExist(song))
            {
                _logger.Info("Cannot delete the year record because it does not exist");
                return;
            }

            var year = yearStore.GetSongYear(song, true);

            if (year.SongCount <= 1)
                yearStore.DeleteYear(year);
        }
        #endregion    


        #region Structs
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct Sng
        {
            public int Id;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
            public string Title;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
            public string Artist;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
            public string Album;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
            public string Genre;
            public int Year;
            public int Duration;
            [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 1024)]
            public string SongPath;
        };
        #endregion
    }
}
