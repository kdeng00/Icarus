using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.IO;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Configuration;

using NLog;

using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.Managers
{
    public class SongManager : BaseManager
    {
        #region Fields
        private IConfiguration _config;
        private string _connectionString;
        private string _tempDirectoryRoot;
        private string _archiveDirectoryRoot;
        private string _compressedSongFilename;
        private string _message;
        #endregion


        #region Properties
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
        /**
        public SongManager()
        {
            Initialize();
            InitializeConnection();
        }
        */

        public SongManager(IConfiguration config)
        {
            _config = config;
            Initialize();
        }
        public SongManager(IConfiguration config, string tempDirectoryRoot)
        {
            _config = config;
            _tempDirectoryRoot = tempDirectoryRoot;
            Initialize();
        }
        #endregion


        #region Methods
        public SongResult UpdateSong(Song song)/**, SongRepository songStore, AlbumRepository albumStore,
                ArtistRepository artistStore, GenreRepository genreStore, YearRepository yearStore)*/
        {
            var result = new SongResult();

            try
            {
                var oldSongRecord = songStore.GetSong(song);
                song.SongPath = oldSongRecord.SongPath;

                MetadataRetriever updateMetadata = new MetadataRetriever();
                updateMetadata.UpdateMetadata(song, oldSongRecord);

                var updatedSong = updateMetadata.UpdatedSongRecord;

                var updatedAlbum = UpdateAlbumInDatabase(oldSongRecord, updatedSong)
                oldSongRecord.AlbumId = updatedAlbum.AlbumId;

                var updatedArtist = UpdateArtistInDatabase(oldSongRecord, updatedSong);
                oldSongRecord.ArtistId = updatedArtist.ArtistId;

                var updatedGenre = UpdateGenreInDatabase(oldSongRecord, updatedSong);
                Console.WriteLine($"Old Genre Id {oldSongRecord.GenreId}");
                oldSongRecord.GenreId = updatedGenre.GenreId;
                Console.WriteLine($"Updated Genre Id {updatedGenre.GenreId}");

                UpdateSongInDatabase(ref oldSongRecord, ref updatedSong, ref result);

                DeleteEmptyDirectories(ref oldSongRecord, ref updatedSong);
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

        public bool DeleteSongFromFileSystem(Song songMetaData)
        {
            bool successful = false;
            try
            {
                var songPath = songMetaData.SongPath;
                System.IO.File.Delete(songPath);
                successful = true;
                DirectoryManager dirMgr = new DirectoryManager(_config, songMetaData);
                dirMgr.DeleteEmptyDirectories();
                Console.WriteLine("Song successfully deleted");
            }
            catch (Exception ex)
            {
                var exMsg = ex.Message;
            }

            return successful;
        }

        public void DeleteSong(Song song)/**, SongRepository songStore, 
                AlbumRepository albumStore, ArtistRepository artistStore, 
                GenreRepository genreStore, YearRepository yearStore, 
                CoverArtRepository coverStore)*/
        {
            try
            {
                if (DeleteSongFromFilesystem(song))
                {
                    _logger.Error("Failed to delete the song");

                    throw new Exception("Failed to delete the song");
                }
                _logger.Info("Song deleted from the filesystem");

                var coverMgr = new CoverArtManager(_config.GetValue<string>(
                            "CoverArtPath"));
                var coverArt = coverStore.GetCoverArt(song);
                coverMgr.DeleteCoverArt(coverArt);

                coverMgr.DeleteCoverArtFromDatabase(coverArt);
                DeleteSongFromDatabase(song/**, songStore, albumStore, artistStore,
                        genreStore, yearStore*/);
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
        }


        
        
        
        public async Task SaveSongToFileSystem(IFormFile songFile/**, SongRepository songStore,
                AlbumRepository albumStore, ArtistRepository artistStore,
                GenreRepository genreStore, YearRepository yearStore,
                CoverArtRepository coverArtStore*/)
        {
            try
            {
                _logger.Info("Starting the process of saving the song to the filesystem");

                var fileTempPath = Path.Combine(_tempDirectoryRoot, songFile.FileName);
                var song = await SaveSongTemp(songFile, fileTempPath);
                song.SongPath = fileTempPath;

                DirectoryManager dirMgr = new DirectoryManager(_config, song);
                dirMgr.CreateDirectory();

                System.IO.File.Delete(fileTempPath);

                var filePath = dirMgr.SongDirectory;
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

                coverMgr.SaveCoverArtToDatabase(ref song, ref coverArt);//, 
                SaveSongToDatabase(song);/**, songStore, albumStore, artistStore, genreStore,
                        yearStore);*/
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



        private async Task<SongData> RetrieveSongFromFileSystem(Song details)
        {
            byte[] uncompressedSong = System.IO.File.ReadAllBytes(details.SongPath);
            
            return new SongData
            {
                Data = uncompressedSong
            };
        }
        private async Task<Song> SaveSongTemp(IFormFile songFile, string filePath)
        {
            var song = new Song();


            using (var filestream = new FileStream(filePath, FileMode.Create))
            {
                _logger.Info("Saving song to temporary directory");
                await songFile.CopyToAsync(filestream);
            }

            MetadataRetriever meta = new MetadataRetriever();
            song =  meta.RetrieveMetaData(filePath);

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

        private void DeleteEmptyDirectories(ref Song oldSong, ref Song updatedSong)
        {
            DirectoryManager mgr = new DirectoryManager(_config);

            _logger.Info("Checking to see if there are any directories to delete");
            mgr.DeleteEmptyDirectories(oldSong);
        }

        private void Initialize()
        {
            try
            {
                _connectionString = _config.GetConnectionString("IcarusDev");
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error Occurred: {ex.Message}");
            }
            
        }
        

        
        private void SaveSongToDatabase(Song song)/**, SongRepository songStore, AlbumRepository albumStore,
                ArtistRepository artistStore, GenreRepository genreStore, YearRepository yearStore)*/
        {
            _logger.Info("Starting process to save the song to the database");

            SaveAlbumToDatabase(ref song);//, albumStore);
            SaveArtistToDatabase(ref song);//, artistStore);
            SaveGenreToDatabase(ref song);//, genreStore);

            var info = "Saving Song to DB";
            Console.WriteLine(info);
            _logger.Info(info);
            songStore.SaveSong(song);
        }
        private void SaveAlbumToDatabase(ref Song song)//, AlbumRepository albumStore)
        {
            _logger.Info("Starting process to save the album record of the song to the database");

            var album = new Album();

            album.Title = song.AlbumTitle;
            album.AlbumArtist = song.Artist;

            if (!albumStore.DoesAlbumExist(song))
            {
                album.SongCount = 1;
                albumStore.SaveAlbum(album);
                album = albumStore.GetAlbum(song);
                Console.WriteLine($"Album Id {album.AlbumId}");
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
        private void SaveArtistToDatabase(ref Song song)//, ArtistRepository artistStore)
        {
            _logger.Info("Starting process to save the artist record of the song to the database");

            var artist = new Artist();

            artist.Name = song.Artist;
            artist.SongCount = 1;

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
        private void SaveGenreToDatabase(ref Song song)//, GenreRepository genreStore)
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
                Console.WriteLine("Going to find genre");
                genre = genreStore.GetGenre(song);
                var genreDump = $"Genre id {genre.GenreId} GenreName {genre.GenreName}" +
                    $" Genre song Count {genre.SongCount}";
                Console.WriteLine(genreDump);
                _logger.Info(genreDump);
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
        

        private bool DeleteSongFromFilesystem(Song song)
        {
            var songPath = song.SongPath;

            _logger.Info("Deleting song from the filesystem");

            try
            {
                System.IO.File.Delete(songPath);
            }
            catch(Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred when attempting to delete the song from the filesystem");
                return false;
            }

            return DoesSongExistOnFilesystem(song);
        }
        private bool DoesSongExistOnFilesystem(Song song)
        {
            var songPath = song.SongPath;

            if (!System.IO.File.Exists(songPath))
            {
                _logger.Info("Song does not exist on the filesystem");

                return false;
            }

            _logger.Info("Song exists on the filesystem");

            return true;
        }

        private Album UpdateAlbumInDatabase(Song oldSongRecord, Song newSongRecord)//, AlbumRepository albumStore)
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

                var newAlbumRecord = new Album
                {
                    Title = newAlbumTitle,
                    AlbumArtist = newAlbumArtist
                };

                albumStore.SaveAlbum(newAlbumRecord);

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
        private Artist UpdateArtistInDatabase(Song oldSongRecord, Song newSongRecord)//, ArtistRepository artistStore)
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
        private Genre UpdateGenreInDatabase(Song oldSongRecord, Song newSongRecord)//, GenreRepository genreStore)
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
        
        private void UpdateSongInDatabase(ref Song oldSongRecord, ref Song newSongRecord, /**SongRepository songStore,*/
                ref SongResult result)
        {
            var updatedSongRecord = new Song
            {
                SongID = oldSongRecord.SongID,
                Title = oldSongRecord.Title,
                Artist = oldSongRecord.Artist,
                AlbumTitle = oldSongRecord.AlbumTitle,
                Genre = oldSongRecord.Genre,
                Year = oldSongRecord.Year,
                Filename = oldSongRecord.Filename,
                SongPath = oldSongRecord.SongPath,
                ArtistId = oldSongRecord.ArtistId,
                AlbumId = oldSongRecord.AlbumId,
                GenreId = oldSongRecord.GenreId,
                YearId = oldSongRecord.YearId

            };
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
                Console.WriteLine("Genre changed");
                Console.WriteLine($"{updatedSongRecord.Genre} {newSongRecord.Genre}");
            }
            if (newSongRecord.Year != null || newSongRecord.Year > 0)
                updatedSongRecord.Year = newSongRecord.Year;

            _logger.Info("Applied changes to song record");

            if (artistOrAlbumChanged)
            {
                _logger.Info("Change to song's album or artist");

                DirectoryManager dirMgr = new DirectoryManager(_config);
                var oldSongPath = updatedSongRecord.SongPath;
                var newSongPath = dirMgr.GenerateSongPath(updatedSongRecord);
                var filename = updatedSongRecord.Filename;

                Console.WriteLine($"Old song path {oldSongPath}");
                Console.WriteLine($"New song path {newSongPath}");

                _logger.Info("Copying song to the new path");

                System.IO.File.Copy(oldSongPath, newSongPath + filename, true);

                _logger.Info("Checking to see if song successfully copied");

                if (!System.IO.File.Exists(newSongPath + filename))
                {
                    _logger.Info("Song did not successfully copy");

                    Console.WriteLine("New path does not exist when it should");
                }

                updatedSongRecord.SongPath = newSongPath + filename;
                
                _logger.Info("Deleting old song path");

                System.IO.File.Delete(oldSongPath);

                if (System.IO.File.Exists(oldSongPath))
                    Console.WriteLine("Old path exists when it should not");
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

        private void DeleteSongFromDatabase(Song song)/**, SongRepository songStore, AlbumRepository albumStore,
                ArtistRepository artistStore, GenreRepository genreStore, YearRepository yearStore)*/
        {
            _logger.Info("Starting process to delete records related to the song from the database");

            DeleteAlbumFromDatabase(song);//, albumStore);
            DeleteArtistFromDatabase(song);//, artistStore);
            DeleteGenreFromDatabase(song);//, genreStore);

            var sngContext = new SongContext(_connectionString);
            sngContext.Songs.Remove(song);
            sngContext.SaveChanges();
        }
        private void DeleteAlbumFromDatabase(Song song)// AlbumRepository albumStore)
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
        private void DeleteArtistFromDatabase(Song song)//, ArtistRepository artistStore)
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
        private void DeleteGenreFromDatabase(Song song)//, GenreRepository genreStore)
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
        

        #endregion    
    }
}
