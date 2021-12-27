using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
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
        private string _tempDirectoryRoot;
        private string _archiveDirectoryRoot;
        private string _compressedSongFilename;
        private string _message;
        private SongContext _songContext;
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
        public SongResult UpdateSong(Song song)
        {
            var result = new SongResult();
            if (!DoesSongExist(song))
            {
                result.SongTitle = song.Title;
                result.Message = "Song does not exist";
                return result;
            }

            try
            {
                var oldSongRecord = _songContext.RetrieveRecord(song);
                song.Filename = oldSongRecord.Filename;
                song.SongDirectory = oldSongRecord.SongDirectory;

                MetadataRetriever updateMetadata = new MetadataRetriever();
                updateMetadata.UpdateMetadata(song, oldSongRecord);

                var updatedSong = updateMetadata.UpdatedSongRecord;

                var albMgr = new AlbumManager(_config);
                var gnrMgr = new GenreManager(_config);
                var artMgr = new ArtistManager(_config);
                var updatedAlbum = albMgr.UpdateAlbumInDatabase(oldSongRecord, updatedSong);
                oldSongRecord.AlbumID = updatedAlbum.AlbumID;

                var updatedArtist = artMgr.UpdateArtistInDatabase(oldSongRecord, updatedSong);
                oldSongRecord.ArtistID = updatedArtist.ArtistID;

                var updatedGenre = gnrMgr.UpdateGenreInDatabase(oldSongRecord, updatedSong);
                oldSongRecord.GenreID = updatedGenre.GenreID;

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
                var songPath = songMetaData.SongPath();
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

        public bool DoesSongExist(Song song)
        {
            if (!_songContext.DoesRecordExist(song))
            {
                return false;
            }

            return true;
        }
        public void DeleteSong(Song song)
        {
            try
            {
                if (DeleteSongFromFilesystem(song))
                {
                    _logger.Error("Failed to delete the song");

                    throw new Exception("Failed to delete the song");
                }
                _logger.Info("Song deleted from the filesystem");

                var coverMgr = new CoverArtManager(_config);

                var coverArt = coverMgr.GetCoverArt(song);
                coverMgr.DeleteCoverArt(coverArt);

                coverMgr.DeleteCoverArtFromDatabase(coverArt);
                DeleteSongFromDatabase(song);
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
        }


        public async Task SaveSongToFileSystem(IFormFile songFile)
        {
            try
            {
                _logger.Info("Starting the process of saving the song to the filesystem");

                var song = await SaveSongTemp(songFile);
                System.IO.File.Delete(song.SongPath());

                DirectoryManager dirMgr = new DirectoryManager(_config, song);
                dirMgr.CreateDirectory();

                var fileDir = dirMgr.SongDirectory;
                var filePath = fileDir;
                var songFilename = song.Filename;

                if (!songFilename.EndsWith(".mp3"))
                    filePath += $"{songFilename}.mp3";
                else
                    filePath += $"{songFilename}";

                _logger.Info($"Absolute song path: {filePath}");


                using (var fileStream = new FileStream(filePath, FileMode.Create))
                {
                    await (songFile.CopyToAsync(fileStream));
                    song.SongDirectory = fileDir;

                    _logger.Info("Song successfully saved to filesystem");
                }

                var coverMgr = new CoverArtManager(_config);
                var coverArt = coverMgr.SaveCoverArt(song);

                coverMgr.SaveCoverArtToDatabase(ref song, ref coverArt);
                SaveSongToDatabase(song);
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }
        }

        
        
        public async Task<SongData> RetrieveSong(Song songMetaData)
        {
            var song = new SongData();

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
            byte[] uncompressedSong = await System.IO.File.ReadAllBytesAsync(details.SongPath());
            
            return new SongData
            {
                Data = uncompressedSong
            };
        }
        private async Task<Song> SaveSongTemp(IFormFile songFile)
        {
            var song = new Song();
            var filename = song.GenerateFilename();
            var filePath = Path.Combine(_tempDirectoryRoot, filename);


            using (var filestream = new FileStream(filePath, FileMode.Create))
            {
                _logger.Info("Saving song to temporary directory");
                await songFile.CopyToAsync(filestream);
            }

            MetadataRetriever meta = new MetadataRetriever();
            song =  meta.RetrieveMetaData(filePath);

            _logger.Info("Assigning song filename");
            song.Filename = filename;
            song.SongDirectory = _tempDirectoryRoot;
            song.DateCreated = DateTime.Now;

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
                _connectionString = _config.GetConnectionString("DefaultConnection");
                _songContext = new SongContext(_connectionString);
            }
            catch (Exception ex)
            {
                Console.WriteLine($"Error Occurred: {ex.Message}");
            }
        }
        

        
        private void SaveSongToDatabase(Song song)
        {
            _logger.Info("Starting process to save the song to the database");

            var albumMgr = new AlbumManager(_config);
            var artistMgr = new ArtistManager(_config);
            var genreMgr = new GenreManager(_config);
            albumMgr.SaveAlbumToDatabase(ref song);
            artistMgr.SaveArtistToDatabase(ref song);
            genreMgr.SaveGenreToDatabase(ref song);

            var info = "Saving Song to DB";
            Console.WriteLine(info);
            _logger.Info(info);
            _songContext.Add(song);
            _songContext.SaveChanges();
        }
        

        private bool DeleteSongFromFilesystem(Song song)
        {
            var songPath = song.SongPath();

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
            if (!System.IO.File.Exists(song.SongPath()))
            {
                _logger.Info("Song does not exist on the filesystem");

                return false;
            }

            _logger.Info("Song exists on the filesystem");

            return true;
        }

        
        private void UpdateSongInDatabase(ref Song oldSongRecord, ref Song newSongRecord, ref SongResult result)
        {
            var updatedSongRecord = oldSongRecord;

            var songContext = new SongContext(_connectionString);

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
            }
            if (!string.IsNullOrEmpty(newSongRecord.Artist))
            {
                updatedSongRecord.Artist = newSongRecord.Artist;
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


            _logger.Info("Saving song metadata to the database");

            songContext.Update(updatedSongRecord);
            songContext.SaveChanges();

            newSongRecord = updatedSongRecord;

            result.Message = "Successfully updated song";
            result.SongTitle = updatedSongRecord.Title;
        }

        private void DeleteSongFromDatabase(Song song)
        {
            _logger.Info("Starting process to delete records related to the song from the database");

            var albumMgr = new AlbumManager(_config);
            var artistMgr = new ArtistManager(_config);
            var genreMgr = new GenreManager(_config);
            albumMgr.DeleteAlbumFromDatabase(song);
            artistMgr.DeleteArtistFromDatabase(song);
            genreMgr.DeleteGenreFromDatabase(song);

            var sngContext = new SongContext(_connectionString);
            sngContext.Songs.Remove(song);
            sngContext.SaveChanges();
        }
        #endregion    
    }
}
