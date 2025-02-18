using NLog;

using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;
using TagLib.Mpeg4;

namespace Icarus.Controllers.Managers;


public class SongManager : BaseManager
{
    #region Fields
    private string? _tempDirectoryRoot;
    private string? _archiveDirectoryRoot;
    private string? _compressedSongFilename;
    private string? _message;
    private SongContext? _songContext;
    #endregion


    #region Properties
    public string? ArchiveDirectoryRoot
    {
        get => _archiveDirectoryRoot;
        set => _archiveDirectoryRoot = value;
    }
    public string? CompressedSongFilename
    {
        get => _compressedSongFilename;
        set => _compressedSongFilename = value;
    }
    public string? Message
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
            var oldSongRecord = _songContext!.RetrieveRecord(song);
            song.Filename = oldSongRecord.Filename;
            song.SongDirectory = oldSongRecord.SongDirectory;

            MetadataRetriever updateMetadata = new MetadataRetriever();
            updateMetadata.UpdateMetadata(song, oldSongRecord);

            var updatedSong = updateMetadata.UpdatedSongRecord;

            var albMgr = new AlbumManager(_config!);
            var gnrMgr = new GenreManager(_config!);
            var artMgr = new ArtistManager(_config!);
            var updatedAlbum = albMgr.UpdateAlbumInDatabase(oldSongRecord, updatedSong!);
            oldSongRecord.AlbumId = updatedAlbum.Id;

            var updatedArtist = artMgr.UpdateArtistInDatabase(oldSongRecord, updatedSong!);
            oldSongRecord.ArtistId = updatedArtist.Id;

            var updatedGenre = gnrMgr.UpdateGenreInDatabase(oldSongRecord, updatedSong!);
            oldSongRecord.GenreId = updatedGenre.Id;

            UpdateSongInDatabase(ref oldSongRecord, ref updatedSong!, ref result);

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
            successful = !System.IO.File.Exists(songPath);
            if (successful)
            {
                Console.WriteLine("Song successfully deleted");
            }
            DirectoryManager dirMgr = new DirectoryManager(_config!, songMetaData);
            var deletedAmount = dirMgr.DeleteEmptyDirectories(songMetaData.SongDirectory, 1);
            if (deletedAmount > 0)
            {
                Console.WriteLine($"{deletedAmount} directories deleted");
            }
        }
        catch (Exception ex)
        {
            var exMsg = ex.Message;
        }

        return successful;
    }

    public bool DoesSongExist(Song song)
    {
        if (!_songContext!.DoesRecordExist(song))
        {
            return false;
        }

        return true;
    }
    public void DeleteSong(Song song)
    {
        try
        {
            if (DeleteSongFromFilesystem(song, true))
            {
                _logger.Error("Failed to delete the song");

                throw new Exception("Failed to delete the song");
            }
            _logger.Info("Song deleted from the filesystem");

            var coverMgr = new CoverArtManager(_config!);

            var coverArt = coverMgr.GetCoverArt(song);
            coverMgr.DeleteCoverArt(coverArt);

            DeleteSongFromDatabase(song);
            coverMgr.DeleteCoverArtFromDatabase(coverArt);
        }
        catch (Exception ex)
        {
            var msg = ex.Message;
            _logger.Error(msg, "An error occurred");
        }
    }


    // TODO: Delete
    public async Task SaveSongToFileSystem(IFormFile songFile)
    {
        try
        {
            _logger.Info("Starting the process of saving the song to the filesystem");

            var song = await SaveSongTemp(songFile);

            DirectoryManager dirMgr = new DirectoryManager(_config!, song);
            dirMgr.CreateDirectory();

            var tempPath = song.SongPath();
            song.Filename = song.GenerateFilename(true, AudioFileExtensionsType.WAV);
            var filePath = $"{dirMgr.SongDirectory}{song.Filename}";

            _logger.Info($"Absolute song path: {filePath}");


            await Task.Run(() =>
            {
                this.MoveSongToFinalDestination(tempPath, filePath);
            });

            song.SongDirectory = dirMgr.SongDirectory;

            var coverMgr = new CoverArtManager(_config!);
            var coverArt = coverMgr.SaveCoverArt(song);

            SaveSongToDatabase(song, coverArt);
        }
        catch (Exception ex)
        {
            var msg = ex.Message;
            _logger.Error(msg, "An error occurred");
        }
    }

    // Change the name of this method to only focus on wav files
    [Obsolete("Support for uplodaing wav files will end. Use the flac alternative instead - SaveFlacSongToFileSystem(..)")]
    public Song SaveSongToFileSystem(IFormFile songFile, IFormFile coverArtData, Song song)
    {
        if (string.IsNullOrEmpty(song.SongDirectory))
        {
            song.SongDirectory = _tempDirectoryRoot;
        }

        if (string.IsNullOrEmpty(song.Filename))
        {
            switch (song.AudioType)
            {
                case "wav":
                    song.Filename = song.GenerateFilename(true, AudioFileExtensionsType.WAV);
                    break;
                case "flac":
                    song.Filename = song.GenerateFilename(true, AudioFileExtensionsType.FLAC);
                    break;
                default:
                    song.Filename = song.GenerateFilename(true, AudioFileExtensionsType.Default);
                    break;
            }
        }

        _logger.Info($"Temporary directory: {_tempDirectoryRoot}");

        var tempPath = song.SongPath();

        _logger.Info("Temporary song path: {0}", tempPath);

        if (!System.IO.File.Exists(tempPath))
        {
            using (var filestream = new FileStream(tempPath, FileMode.Create))
            {
                _logger.Info("Saving song to temporary directory");
                songFile.CopyTo(filestream);
            }
        }

        var coverMgr = new CoverArtManager(_config!);
        var coverArt = coverMgr.SaveCoverArt(coverArtData, song);

        DirectoryManager dirMgr = new DirectoryManager(_config!, song);
        dirMgr.CreateDirectory();

        song.SongDirectory = dirMgr.SongDirectory;

        var filePath = song.SongPath();
        _logger.Info($"Absolute song path: {filePath}");

        this.MoveSongToFinalDestination(tempPath, filePath);

        SaveSongToDatabase(song, coverArt);

        return song;
    }

    public Song SaveFlacSongToFileSystem(IFormFile songFile, IFormFile coverArtData, Song song)
    {
        // Save temp song (Should already be saved to the filesystem by the time it gets to this method)
        // Save cover art
        // Update the song's metadata with the song object
        // Save song to its final directory
        // Save cover art to the database
        // Save song to the database

        var coverMgr = new CoverArtManager(_config!);
        var coverArt = coverMgr.SaveCoverArt(coverArtData, song);

        var meta = new Utilities.MetadataRetriever();
        meta.UpdateMetadata(song, song);

        DirectoryManager dirMgr = new DirectoryManager(_config!, song);
        dirMgr.CreateDirectory();

        var tempPath = song.SongPath();

        song.SongDirectory = dirMgr.SongDirectory;
        song.Filename = song.GenerateFilename(true, AudioFileExtensionsType.FLAC);

        var filePath = song.SongPath();
        _logger.Info($"Absolute song path: {filePath}");

        this.MoveSongToFinalDestination(tempPath, filePath);

        SaveSongToDatabase(song, coverArt);

        return song;
    }

    private void MoveSongToFinalDestination(string sourcePath, string targetPath)
    {
        using (var fileStream = new FileStream(targetPath, FileMode.Create))
        {
            var songBytes = System.IO.File.ReadAllBytes(sourcePath);

            try
            {
                if (System.IO.File.Exists(sourcePath) && System.IO.File.Exists(sourcePath) && fileStream.Length > 0)
                {
                    System.IO.File.Delete(sourcePath);
                    _logger.Info("Deleted temp song from filesystem: {0}", sourcePath);
                }
                else
                {
                    fileStream.Write(songBytes, 0, songBytes.Count());
                    _logger.Info("Saved song to filesystem: {0}", targetPath);

                    System.IO.File.Delete(sourcePath);
                    _logger.Info("Deleted temp song from filesystem: {0}", sourcePath);
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error($"An error occurred: {msg}");
            }

            _logger.Info("Song successfully saved to filesystem");
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
    public async Task<Song> SaveSongTemp(IFormFile songFile)
    {
        _logger.Info("Assigning song filename");
        var song = new Song { SongDirectory = this._tempDirectoryRoot! };
        var filename = await song.GenerateFilenameAsync(false) + "-" + songFile.FileName;
        song.Filename = filename;
        var songPath = song.SongPath();

        using (var filestream = new FileStream(songPath, FileMode.Create))
        {
            _logger.Info("Saving temp song: {0}", songPath);
            await songFile.CopyToAsync(filestream);
        }

        song.DateCreated = DateTime.Now;

        return song;
    }


    public Icarus.Models.CreateFileResult Create(IFormFile file, string filePath, string prompt)
    {
        if (System.IO.File.Exists(filePath))
        {
            return CreateFileResult.AlreadyExists;
        }

        using (var filestream = new FileStream(filePath, FileMode.Create))
        {
            Console.WriteLine(prompt);
            file.CopyTo(filestream);

            if (System.IO.File.Exists(filePath))
            {
                return CreateFileResult.FileCreatedAndExists;
            }
        }

        return 0;
    }


    private bool SongRecordChanged(Song currentSong, Song songUpdates)
    {
        var currentTitle = currentSong.Title;
        var currentArtist = currentSong.Artist;
        var currentAlbum = currentSong.AlbumTitle;
        var currentGenre = currentSong.Genre;
        var currentYear = currentSong.Year;

        if (!currentTitle!.Equals(songUpdates.Title) || !currentArtist!.Equals(songUpdates.Artist) ||
                !currentAlbum!.Equals(songUpdates.AlbumTitle) ||
                !currentGenre!.Equals(songUpdates.Genre) || currentYear != songUpdates.Year)
            return true;

        return false;
    }

    private void DeleteEmptyDirectories(ref Song oldSong, ref Song updatedSong)
    {
        DirectoryManager mgr = new DirectoryManager(_config!);

        _logger.Info("Checking to see if there are any directories to delete");
        mgr.DeleteEmptyDirectories(oldSong);
    }

    private void Initialize()
    {
        try
        {
            _connectionString = _config!.GetConnectionString("DefaultConnection");
            _songContext = new SongContext(_connectionString!);
        }
        catch (Exception ex)
        {
            Console.WriteLine($"Error Occurred: {ex.Message}");
        }
    }



    private void SaveSongToDatabase(Song song, CoverArt? cover)
    {
        _logger.Info("Starting process to save the song to the database");

        var albumMgr = new AlbumManager(_config!);
        var artistMgr = new ArtistManager(_config!);
        var genreMgr = new GenreManager(_config!);
        var coverMgr = new CoverArtManager(_config!);
        albumMgr.SaveAlbumToDatabase(ref song);
        artistMgr.SaveArtistToDatabase(ref song);
        genreMgr.SaveGenreToDatabase(ref song);

        var info = "Saving Song to DB";
        _logger.Info(info);

        _songContext!.Add(song);
        _songContext!.SaveChanges();

        coverMgr.SaveCoverArtToDatabase(ref song, ref cover!);
    }


    private bool DeleteSongFromFilesystem(Song song, bool deleteDirectory = false)
    {
        var songPath = song.SongPath();

        _logger.Info("Deleting song from the filesystem");

        try
        {
            System.IO.File.Delete(songPath);

            DeleteEmptyDirectories(ref song, ref song);
        }
        catch (Exception ex)
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

        var songContext = new SongContext(_connectionString!);

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
        result.SongTitle = updatedSongRecord.Title!;
    }

    private void DeleteSongFromDatabase(Song song)
    {
        _logger.Info("Starting process to delete records related to the song from the database");

        var albumMgr = new AlbumManager(_config!);
        var artistMgr = new ArtistManager(_config!);
        var genreMgr = new GenreManager(_config!);
        var sngContext = new SongContext(_connectionString!);
        sngContext.Songs!.Remove(song);
        sngContext.SaveChanges();
        artistMgr.DeleteArtistFromDatabase(song);
        albumMgr.DeleteAlbumFromDatabase(song);
        genreMgr.DeleteGenreFromDatabase(song);
    }
    #endregion    
}
