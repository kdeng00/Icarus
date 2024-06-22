using System;
using System.Collections.Generic;

using Microsoft.Extensions.Logging;
using NLog;
using TagLib;

using Icarus.Models;

namespace Icarus.Controllers.Utilities;

public class MetadataRetriever
{
    #region Fields
    private static NLog.Logger _logger = NLog.Web.NLogBuilder.ConfigureNLog("nlog.config").GetCurrentClassLogger();
    private Song _updatedSong;
    private string _message;
    private string _title;
    private string _artist;
    private string _album;
    private string _genre;
    private int _year;
    private int _duration;
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


    #region Constructors
    #endregion


    #region Methods
    public static void PrintMetadata(Song song)
    {
        Console.WriteLine("\n\nMetadata of the song:");
        Console.WriteLine($"ID: {song.SongID}");
        Console.WriteLine($"Title: {song.Title}");
        Console.WriteLine($"Artist: {song.Artist}");
        Console.WriteLine($"Album: {song.AlbumTitle}");
        Console.WriteLine($"Genre: {song.Genre}");
        Console.WriteLine($"Year: {song.Year}");
        Console.WriteLine($"Duration: {song.Duration}");
        Console.WriteLine($"AlbumID: {song.AlbumID}");
        Console.WriteLine($"ArtistID: {song.ArtistID}");
        Console.WriteLine($"GenreID: {song.GenreID}");
        Console.WriteLine($"Song Path: {song.SongPath()}");
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

    public string CoverArtFileExtensionType(CoverArt cover)
    {
        Console.WriteLine("Retrieving CoverArt file extension type");

        using (var fileStream = System.IO.File.OpenRead(cover.ImagePath()))
        {
            var isRecognizableType = FileTypeChecker.FileTypeValidator.IsTypeRecognizable(fileStream);

            if (!isRecognizableType)
            {
                return string.Empty;
            }

            var fileType = FileTypeChecker.FileTypeValidator.GetFileType(fileStream);

            Console.WriteLine($"Filetype: {fileType}");

            return fileType.Extension;
        }
    }

    public string FileExtensionType(IFormFile file)
    {
        using (var fileStream = file.OpenReadStream())
        {
            var isRecognizableType = FileTypeChecker.FileTypeValidator.IsTypeRecognizable(fileStream);

            if (!isRecognizableType)
            {
                return string.Empty;
            }


            var fileType = FileTypeChecker.FileTypeValidator.GetFileType(fileStream);
            Console.WriteLine($"Filetype: {fileType}");
            Console.WriteLine($"Extension: {fileType.Extension}");

            return fileType.Extension;
        }
    }

    public Song RetrieveMetaData(string filePath)
    {
        Song song = new Song();

        try
        {
            TagLib.File fileTag = TagLib.File.Create(filePath);
            _title = fileTag.Tag.Title;
            _artist = string.Join("", fileTag.Tag.Performers);
            _album = fileTag.Tag.Album;
            _genre = string.Join("", fileTag.Tag.Genres);
            _year = (int)fileTag.Tag.Year;
            _duration = (int)fileTag.Properties.Duration.TotalSeconds;
            var albumArtist = string.Join("", fileTag.Tag.AlbumArtists);
            var track = (int)fileTag.Tag.Track;
            var disc = (int)fileTag.Tag.Disc;

            song.Title = _title;
            song.Artist = _artist;
            song.AlbumTitle = _album;
            song.AlbumArtist = albumArtist;
            song.Genre = _genre;
            song.Year = _year;
            song.Duration = _duration;
            song.Track = track;
            song.Disc = disc;
            song.TrackCount = (int)fileTag.Tag.TrackCount;
            song.DiscCount = (int)fileTag.Tag.DiscCount;
        }
        catch (Exception ex)
        {
            var msg = ex.Message;
            Console.WriteLine("An error occurred in MetadataRetriever");
            Console.WriteLine(msg);
            _logger.Error(msg, "An error occurred in MetadataRetriever");
        }

        return song;
    }

    public int RetrieveSongDuration(string filepath)
    {
        var duration = 0;
        var fileTag = TagLib.File.Create(filepath);
        duration = (int)fileTag.Properties.Duration.TotalSeconds;

        return duration;
    }

    public byte[] RetrieveCoverArtBytes(Song song)
    {
        try
        {
            Console.WriteLine("Fetching image");
            var tag = TagLib.File.Create(song.SongPath());
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

        var tag = TagLib.File.Create(song.SongPath());
        var pics = tag.Tag.Pictures;
        Array.Resize(ref pics, 1);

        pics[0] = new Picture(coverArt.ImagePath())
        {
            Description = "Cover Art"
        };

        tag.Tag.Pictures = pics;
        tag.Save();
    }

    private void PerformUpdate(Song updatedSong, SortedDictionary<string, bool> checkedValues)
    {
        var filePath = updatedSong.SongPath();
        var title = updatedSong.Title;
        var artist = updatedSong.Artist;
        var album = updatedSong.AlbumTitle;
        var genre = updatedSong.Genre;
        var year = updatedSong.Year;
        var albumArtist = updatedSong.AlbumArtist;
        var track = updatedSong.Track;
        var trackCount = updatedSong.TrackCount;
        var disc = updatedSong.Disc;
        var discCount = updatedSong.DiscCount;
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
                        case "albumartist":
                            _updatedSong.AlbumArtist = albumArtist;
                            fileTag.Tag.AlbumArtists = new []{albumArtist};
                            break;
                        case "track":
                            _updatedSong.Track = track;
                            fileTag.Tag.Track = (uint)(track);
                            break;
                        case "trackcount":
                            _updatedSong.TrackCount = trackCount;
                            fileTag.Tag.TrackCount = (uint)(trackCount);
                            break;
                        case "disc":
                            _updatedSong.Disc = disc;
                            fileTag.Tag.Disc = (uint)(disc);
                            break;
                        case "disccount":
                            _updatedSong.DiscCount = discCount;
                            fileTag.Tag.DiscCount = (uint)(discCount);
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
        _updatedSong = song;
    }
    private void PrintMetadata()
    {
        Console.WriteLine("\n\nMetadata of the song:");
        Console.WriteLine($"Title: {_title}");
        Console.WriteLine($"Artist: {_artist}");
        Console.WriteLine($"Album: {_album}");
        Console.WriteLine($"Genre: {_genre}");
        Console.WriteLine($"Year: {_year}");
        Console.WriteLine($"Duration: {_duration}\n\n");

        _logger.Info("Metadata of the song");
        _logger.Info($"Title: {_title}");
        _logger.Info($"Artist: {_artist}");
        _logger.Info($"Album: {_album}");
        _logger.Info($"Genre: {_genre}");
        _logger.Info($"Year: {_year}");
        _logger.Info($"Duration: {_duration}");
    }
    private void PrintMetadata(Song song, string message)
    {
        Console.WriteLine($"\n\n{message}");
        Console.WriteLine($"Title: {song.Title}");
        Console.WriteLine($"Artist: {song.Artist}");
        Console.WriteLine($"Album: {song.AlbumTitle}");
        Console.WriteLine($"Genre: {song.Genre}");
        Console.WriteLine($"Year: {song.Year}");
        Console.WriteLine($"Duration: {song.Duration}\n\n");

        _logger.Info(message);
        _logger.Info($"Title: {_title}");
        _logger.Info($"Artist: {_artist}");
        _logger.Info($"Album: {_album}");
        _logger.Info($"Genre: {_genre}");
        _logger.Info($"Year: {_year}");
        _logger.Info($"Duration: {_duration}");
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
            songValues["AlbumArtist"] = String.IsNullOrEmpty(song.AlbumArtist);

            songValues["Year"] = CheckIntField(song.Year);
            songValues["Track"] = CheckIntField(song.Track);
            songValues["TrackCount"] = CheckIntField(song.TrackCount);
            songValues["Disc"] = CheckIntField(song.Disc);
            songValues["DiscCount"] = CheckIntField(song.Disc);

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

    private bool CheckIntField(int? value)
    {
        if (value == null)
        {
            return true;
        }
        else if (value == 0)
        {
            return true;
        }

        return false;
    }
    #endregion
}
