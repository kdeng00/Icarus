using System;
using System.Collections.Generic;
using System.Globalization;

using Microsoft.Extensions.Logging;
using MySql.Data.MySqlClient;
using NLog;

using Icarus.Models;

namespace Icarus.Models.Context
{
	public class ArtistStoreContext : BaseStoreContext
	{
		#region Fields
		private static Logger _logger = NLog.LogManager.GetCurrentClassLogger();
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public ArtistStoreContext(string connectionString)
		{
			_connectionString = connectionString;
		}
		#endregion


		#region Methods
		public List<Artist> GetArtists()
		{
			List<Artist> artists = new List<Artist>();
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "SELECT * FROM Artist";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						using (var reader = cmd.ExecuteReader())
						{

							artists = ParseData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return artists;
		}


		public bool DoesArtistExist(Artist artist)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Artist WHERE ArtistId=@ArtistId";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@AlbumId", artist.ArtistId);
						
						using (var reader = cmd.ExecuteReader())
						{
							artist =  ParseSingleData(reader);

							if (artist.Name != null)
							{
								_logger.Info($"Artist {artist.Name} exists");
								return true;
							}
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}
			return false;
		}
		public bool DoesArtistExist(Song song)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Album WHERE Name=@Name";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Name", song.Artist);
						
						using (var reader = cmd.ExecuteReader())
						{
							var artist =  ParseSingleData(reader);

							if (!String.IsNullOrEmpty(artist.Name))
							{
								_logger.Info($"Artist {artist.Name} exists");
								return true;
							}
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}
			return false;
		}


		public void SaveArtist(Artist artist)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "INSERT INTO Artist(Name, SongCount) " +
						"VALUES(@Name, @SongCount)";

					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Name", artist.Name);
						cmd.Parameters.AddWithValue("@SongCount", artist.SongCount);

						cmd.ExecuteNonQuery();
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}
		}
		public void UpdateArtist(Artist artist)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					// TODO: Implement functionality
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}
		}

		public Artist GetArtist(Artist artist)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Artist WHERE ArtistId=@ArtistId";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@ArtistId", artist.ArtistId);
						using (var reader = cmd.ExecuteReader())
						{
							artist = ParseSingleData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return artist;
		}
		public Artist GetArtist(Song song)
		{
			Artist artist = new Artist();
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Artist WHERE ArtistId=@ArtistId";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@ArtistId", song.ArtistId);
						using (var reader = cmd.ExecuteReader())
						{
							artist = ParseSingleData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return artist;
		}


		private List<Artist> ParseData(MySqlDataReader reader)
		{
			List<Artist> artists = new List<Artist>();

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["ArtistId"]);
				var name = reader["Name"].ToString();
				var songCount = Convert.ToInt32(reader["SongCount"].ToString());
				artists.Add(new Artist
				{
					ArtistId = id,
					Name = name,
					SongCount = songCount
				});
			}

			return artists;
		}

		private Artist ParseSingleData(MySqlDataReader reader)
		{
			Artist artist = new Artist();

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["ArtistId"]);
				var name = reader["Name"].ToString();
				var songCount = Convert.ToInt32(reader["SongCount"].ToString());
				artist.ArtistId = id;
				artist.Name = name;
				artist.SongCount = songCount;
			}

			return artist;
		}
		#endregion
	}
}
