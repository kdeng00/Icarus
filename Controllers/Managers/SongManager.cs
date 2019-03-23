using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.IO;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Configuration;

using Id3;
using Id3.Frames;
using MySql.Data;
using MySql.Data.MySqlClient;
using TagLib;

using Icarus.Models;

namespace Icarus.Controllers.Managers
{
	public class SongManager
	{
		#region Fields
		private List<Song> _songs;
		private Song _song;
		private IConfiguration _config;
		private string _connectionString;
		private string _tempDirectoryRoot;
		#endregion


		#region Properties
		public Song SongDetails
		{
			get
			{
				return _song;
			}
			set
			{
				_song = value;
			}
		}
		#endregion


		#region Constructors
		public SongManager()
		{
			Initialize();
		}

		public SongManager(Song song)
		{
			Initialize();
			_song = song;
		}
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
		public void SaveSongDetails()
		{
			try
			{
				using (MySqlConnection conn = new MySqlConnection(_connectionString))
				{
					conn.Open();
					string query = "INSERT INTO Song(Title, Album, Artist, Year, Genre, Duration, " +
									"Filename, SongPath) VALUES(@Title, @Album, @Artist, @Year, @Genre, " +
									"@Duration, @Filename, @SongPath)";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Title", _song.Title);
						cmd.Parameters.AddWithValue("@Album", _song.Album);
						cmd.Parameters.AddWithValue("@Artist", _song.Artist);
						cmd.Parameters.AddWithValue("@Year", _song.Year);
						cmd.Parameters.AddWithValue("@Genre", _song.Genre);
						cmd.Parameters.AddWithValue("@Duration", _song.Duration);
						cmd.Parameters.AddWithValue("@Filename", _song.Filename);
						cmd.Parameters.AddWithValue("@SongPath", _song.SongPath);

						cmd.ExecuteNonQuery();
					}
				}
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An Error Occurred: {exMsg}");
			}
		}
		public void SaveSongDetails(Song song)
		{
			try
			{
				using (MySqlConnection conn = new MySqlConnection(_connectionString))
				{
					conn.Open();
					string query = "INSERT INTO Song(Title, Album, Artist, Year, Genre, Duration, " +
									", Filename, SongPath) VALUES(@Title, @Album, @Artist, @Year, @Genre, " +
									"@Duration, @Filename, @SongPath)";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Title", song.Title);
						cmd.Parameters.AddWithValue("@Album", song.Album);
						cmd.Parameters.AddWithValue("@Artist", song.Artist);
						cmd.Parameters.AddWithValue("@Year", song.Year);
						cmd.Parameters.AddWithValue("@Genre", song.Genre);
						cmd.Parameters.AddWithValue("@Duration", song.Duration);
						cmd.Parameters.AddWithValue("@Filename", song.Filename);
						cmd.Parameters.AddWithValue("@SongPath", song.SongPath);

						cmd.ExecuteNonQuery();
					}
				}
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An Error Occurred: {exMsg}");
			}
		}
		public async Task SaveSong(SongData songData)
		{
			try
			{
				using (MySqlConnection conn = new MySqlConnection(_connectionString))
				{
					conn.Open();
					string query = "INSERT INTO SongData(Data) VALUES(@Data)";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Data", songData.Data);

						cmd.ExecuteNonQuery();
					}
				}
			}
			catch(Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred: {exMsg}");
			}
		}
		public async Task SaveSongToFileSystem(IFormFile song)
		{
			try
			{
				var filePath = Path.Combine(_tempDirectoryRoot, song.FileName);
				await SaveSongToFileSystemTemp(song, filePath);
				System.IO.File.Delete(filePath);


				DirectoryManager dirMgr = new DirectoryManager(_config, _song);
				dirMgr.CreateDirectory();
				filePath = dirMgr.SongDirectory;
				if (!song.FileName.EndsWith(".mp3"))
					filePath += $"{song.FileName}.mp3";
				else
					filePath += $"{song.FileName}";

				Console.WriteLine($"Full path {filePath}");

				using (var fileStream = new FileStream(filePath, FileMode.Create))
				{
					await song.CopyToAsync(fileStream);
					_song.SongPath = filePath;
					SaveSongDetails();


					Console.WriteLine($"Writing song to the directory: {filePath}");
					Console.WriteLine("Song successfully saved");
				}
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred: {exMsg}");
			}
		}

		public async Task<Song> RetrieveSongDetails(int id)
		{
			DataTable results = new DataTable();

			try
			{
				using (MySqlConnection conn = new MySqlConnection(_connectionString))
				{
					conn.Open();
					string query = "SELECT * FROM Song WHERE Id=@Id";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Id", id);
						cmd.ExecuteNonQuery();

						using (MySqlDataAdapter dataDump = new MySqlDataAdapter(cmd))
						{
							dataDump.Fill(results);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred");
			}
			DataRow row = results.Rows[0];

			return new Song
			{
				Id = Int32.Parse(row["Id"].ToString()),
				Filename = row["Filename"].ToString(),
				SongPath = row["SongPath"].ToString()
			};
		}
		public async Task<SongData> RetrieveSong(int id)
		{
			SongData song = new SongData();
			try
			{
				_song = await RetrieveSongDetails(id);
				song = RetrieveSongFromFileSystem(_song);
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred: {exMsg}");
			}

			return song;
		}


		Song RetrieveMetaData(string filePath)
		{
			string title, artist, album, genre;
			int year, duration;
			
			TagLib.File tfile = TagLib.File.Create(filePath);
			duration = (int)tfile.Properties.Duration.TotalSeconds;


			using (var mp3 = new Mp3(filePath))
    		{
        		Id3Tag tag = mp3.GetTag(Id3TagFamily.Version2X);
				title = tag.Title;
				artist = tag.Artists;
				album = tag.Album;
				genre = "Not Implemented";
				year = (int)tag.Year;
        		Console.WriteLine("Title: {0}", title);
        		Console.WriteLine("Artist: {0}", artist);
        		Console.WriteLine("Album: {0}", album);
				Console.WriteLine("Genre: {0}", genre);
				Console.WriteLine("Year: {0}", year);
				Console.WriteLine("Duration: {0}", duration);
    		}

			_song = new Song
			{
				Title = title,
				Artist = artist,
				Album = album,
				Year = year,
				Genre = genre,
				Duration = duration,
				SongPath = filePath
			};

			return _song;
		}

		SongData RetrieveSongFromFileSystem(Song details)
		{
			
			return new SongData
			{
				Data = System.IO.File.ReadAllBytes(details.SongPath)
			};
		}


		async Task SaveSongToFileSystemTemp(IFormFile song, string filePath)
		{
			using (var fileStream = new FileStream(filePath, FileMode.Create))
			{
				await song.CopyToAsync(fileStream);
				_song = RetrieveMetaData(filePath);
				_song.Filename = song.FileName;
			}
		}

		void Initialize()
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
		#endregion	
	}
}
