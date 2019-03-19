using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Threading.Tasks;

using Microsoft.Extensions.Configuration;

using MySql.Data;
using MySql.Data.MySqlClient;

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
		#endregion


		#region Properties
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
		#endregion


		#region Methods
		public void SaveSongDetails(Song song)
		{
			_song = song;
			try
			{
				Console.WriteLine($"Connection string is: {_connectionString}");
				using (MySqlConnection conn = new MySqlConnection(_connectionString))
				{
					conn.Open();
					string query = "INSERT INTO Song(Title, Album, Artist, Year, Genre, Duration) " +
									"VALUES(@Title, @Album, @Artist, @Year, @Genre, @Duration)";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Title", song.Title);
						cmd.Parameters.AddWithValue("@Album", song.Album);
						cmd.Parameters.AddWithValue("@Artist", song.Artist);
						cmd.Parameters.AddWithValue("@Year", song.Year);
						cmd.Parameters.AddWithValue("@Genre", song.Genre);
						cmd.Parameters.AddWithValue("@Duration", song.Duration);

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

		public Song RetrieveSongDetails(int id)
		{
			return new Song();
		}
		public async Task<SongData> RetrieveSong(int id)
		{
			SongData song = new SongData();
			DataTable results = new DataTable();
			try
			{
				using (MySqlConnection conn = new MySqlConnection(_connectionString))
				{
					conn.Open();
					string query = "SELECT Id, Data From SongData WHERE Id=@Id";
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
				DataRow row = results.Rows[0];
				song.Data = (byte[])row[1];

			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred: {exMsg}");
			}

			return song;
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
