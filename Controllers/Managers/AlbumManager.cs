using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;

using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Utilities;
using Icarus.Models;
using Icarus.Database.Contexts;

namespace Icarus.Controllers.Managers
{
    public class AlbumManager : BaseManager
    {
    	#region Fields
		private AlbumContext _albumContext;
    	#endregion


    	#region Properties
    	#endregion


    	#region Constructors
		public AlbumManager(IConfiguration config)
		{
			_config = config;
			_connectionString = _config.GetConnectionString("DefaultConnection");
			_albumContext = new AlbumContext(_connectionString);
		}
    	#endregion


    	#region Methods
        public void SaveAlbumToDatabase(ref Song song)
        {
            _logger.Info("Starting process to save the album record of the song to the database");

            var album = new Album();

            album.Title = song.AlbumTitle;
            album.AlbumArtist = song.Artist;
            album.Year = song.Year.Value;
            var albumTitle = song.AlbumTitle;
            var albumArtist = song.Artist;

            var albumRetrieved = _albumContext.Albums.FirstOrDefault(alb => alb.Title.Equals(albumTitle) && alb.AlbumArtist.Equals(albumArtist));

            if (albumRetrieved == null)
            {
                album.SongCount = 1;
                _albumContext.Add(album);
                _albumContext.SaveChanges();

                Console.WriteLine($"Album Id {album.AlbumID}");
            }
            else
            {
                album.AlbumID = albumRetrieved.AlbumID;
            }

            song.AlbumID = album.AlbumID;
        }


		public void DeleteAlbumFromDatabase(Song song)
		{
            var album = _albumContext.Albums.FirstOrDefault(alb => alb.Title.Equals(song.AlbumTitle));

            if (album == null)
            {
                _logger.Info("Cannot delete the album record because it does not exist");
                return;
            }

			DeleteAlbumFromDb(album);
        }


		public Album UpdateAlbumInDatabase(Song oldSong, Song newSong)
		{
            var albumRecord = _albumContext.Albums.FirstOrDefault(alb => alb.Title.Equals(oldSong.AlbumTitle));
            var oldAlbumTitle = oldSong.AlbumTitle;
            var oldAlbumArtist = oldSong.Artist;
            var newAlbumTitle = newSong.AlbumTitle;
            var newAlbumArtist = newSong.Artist;

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

            var existingAlbumRecord = _albumContext.Albums.FirstOrDefault(alb => alb.Title.Equals(oldSong.AlbumTitle));
            if (existingAlbumRecord == null)
            {
                _logger.Info("Creating new album record");

                var newAlbumRecord = new Album
                {
                    Title = newAlbumTitle,
                    AlbumArtist = newAlbumArtist,
                    Year = newSong.Year.Value
                };

                _albumContext.Add(newAlbumRecord);
				_albumContext.SaveChanges();

                return newAlbumRecord;
            }
            else
            {
                _logger.Info("Updating existing album record");

            	existingAlbumRecord = _albumContext.Albums.FirstOrDefault(alb => alb.Title.Equals(newSong.AlbumTitle));
                existingAlbumRecord.AlbumArtist = newAlbumArtist;

                _albumContext.Update(existingAlbumRecord);
                _albumContext.SaveChanges();

                return existingAlbumRecord;
            }
        }

		private void DeleteAlbumFromDb(Album album)
		{
			if (SongsInAlbum(album) <= 1)
			{
                _albumContext.Remove(album);
                _albumContext.SaveChanges();
			}
		}

		private int SongsInAlbum(Album album)
		{
			var sngContext = new SongContext(_connectionString);
			var songs = sngContext.Songs.Where(sng => sng.AlbumID == album.AlbumID).ToList();

			return songs.Count;
		}
    	#endregion
    }
}
