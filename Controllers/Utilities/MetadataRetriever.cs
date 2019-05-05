using System;
using System.Collections.Generic;

using TagLib;

using Icarus.Models;

namespace Icarus.Controllers.Utilities
{
	public class MetadataRetriever
	{
		#region Fields
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
		public Song RetrieveMetaData(string filePath)
		{
			Song song = new Song();

			try
			{
				TagLib.File fileTag = TagLib.File.Create(filePath);
				_title = fileTag.Tag.Title;
				_artist = string.Join("", fileTag.Tag.Artists);
				_album = fileTag.Tag.Album;
				_genre = string.Join("", fileTag.Tag.Genres);
				_year = (int)fileTag.Tag.Year;
				_duration = (int)fileTag.Properties.Duration.TotalSeconds;

				song.Title = _title;
				song.Artist = _artist;
				song.Album = _album;
				song.Genre = _genre;
				song.Year = _year;
				song.Duration = _duration;
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				Console.WriteLine("An error occurred in MetadataRetriever");
				Console.WriteLine(msg);
			}

			return song;
		}

		public void UpdateMetadata(Song song)
		{
			try
			{
				Console.WriteLine("Updating song metadata"); 
				var filePath = song.SongPath;
				TagLib.File fileTag = TagLib.File.Create(filePath);
				fileTag.Tag.Title = song.Title;
				fileTag.Tag.Genres = new []{song.Genre};
				fileTag.Save();
				Console.WriteLine("Song metadata updated");

			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				Console.WriteLine($"An error occurred: \n{msg}");
			}
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
				Message = "Failed to update metadata";
			}
		}

		private void PerformUpdate(Song updatedSong, SortedDictionary<string, bool> checkedValues)
		{
			var filePath = updatedSong.SongPath;
			var title = updatedSong.Title;
			var artist = updatedSong.Artist;
			var album = updatedSong.Album;
			var genre = updatedSong.Genre;
			var year = updatedSong.Year;
			TagLib.File fileTag = TagLib.File.Create(filePath);
			try
			{
				Console.WriteLine($"Updating metadata of {title}");
				foreach (var key in checkedValues.Keys)
				{
					bool result = checkedValues[key];
					if (!result)
					{
						switch (key.ToLower())
						{
							case "title":
								_updatedSong.Title = title;
								fileTag.Tag.Title = title;
								break;
							case "artists":
								_updatedSong.Artist = artist;
								fileTag.Tag.Artists = new []{artist};
								break;
							case "album":
								_updatedSong.Album = album;
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
				}
				fileTag.Save();
				Console.WriteLine("Successfully updated metadata");
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				Console.WriteLine($"An error occurred:\n{msg}");
			}
		}
		private void InitializeUpdatedSong(Song song)
		{
			_updatedSong = new Song
			{
				Id = song.Id,
				Title = song.Title,
			     	Album = song.Album,
		     	     	Artist = song.Artist,
				Genre = song.Genre,
				Year = song.Year,
				Duration = song.Duration,
				Filename = song.Filename,
				SongPath = song.SongPath
			};
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
		}
		private void PrintMetadata(Song song, string message)
		{
			Console.WriteLine($"\n\n{message}");
			Console.WriteLine($"Title: {song.Title}");
			Console.WriteLine($"Artist: {song.Artist}");
			Console.WriteLine($"Album: {song.Album}");
			Console.WriteLine($"Genre: {song.Genre}");
			Console.WriteLine($"Year: {song.Year}");
			Console.WriteLine($"Duration: {song.Duration}\n\n");
		}

		private SortedDictionary<string, bool> CheckSongValues(Song song)
		{
			var songValues = new SortedDictionary<string, bool>();
			Console.WriteLine("Checking for null data");
			try
			{
				songValues["Title"] = String.IsNullOrEmpty(song.Title);
				songValues["Artists"] = String.IsNullOrEmpty(song.Artist);
				songValues["Album"] = String.IsNullOrEmpty(song.Album);
				songValues["Genre"] = String.IsNullOrEmpty(song.Genre);
				if (song.Year==0)
				{
					songValues["Year"] = true;
				}
				else
				{
					songValues["Year"] = false;
				}
				Console.WriteLine("Checking for null data completed");
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				Console.WriteLine($"An error occurred: \n{msg}");
			}

			return songValues;
		}	
		#endregion
	}
}
