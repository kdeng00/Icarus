using System;

using TagLib;

using Icarus.Models;

namespace Icarus.Controllers.Utilities
{
	public class MetadataRetriever
	{
		#region Fields
		private string _title;
		private string _artist;
		private string _album;
		private string _genre;
		private int _year;
		private int _duration;
		#endregion


		#region Properties
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
			}
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
		#endregion
	}
}
