using System;
using System.Collections.Generic;
using System.Globalization;

using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Models.Context
{
	public class AlbumStoreContext : BaseStoreContext
	{
		#region Fields
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public AlbumStoreContext(string connectionString)
		{
			_connectionString = connectionString;
		}
		#endregion


		#region Methods
		public List<Album> GetAlbums()
		{
			List<Album> albums = new List<Album>();

			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "SELECT * FROM Album";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						using (var reader = cmd.ExecuteReader())
						{
							while (reader.Read())
							{
								var id = Convert.ToInt32(reader["AlbumId"]);
								var title = reader["Title"].ToString();
								var albumArtist = reader["AlbumArtist"].ToString();
								var songCount = Convert.ToInt32(reader["SongCount"]);
								albums.Add(new Album
								{
									AlbumId = id,
									Title = title,
									AlbumArtist = albumArtist,
									SongCount = songCount
								});
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

			return albums;
		}

		public Album GetAlbum(Album album)
		{

			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "SELECT * FROM Album WHERE AlbumId=@AlbumId";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@AlbumId", album.AlbumId);

						using (var reader = cmd.ExecuteReader())
						{
							while (reader.Read())
							{
								var id = Convert.ToInt32(reader["AlbumId"]);
								var title = reader["Title"].ToString();
								var albumArtist = reader["AlbumArtist"].ToString();
								var songCount = Convert.ToInt32(reader["SongCount"]);
								album =new Album
								{
									AlbumId = id,
									Title = title,
									AlbumArtist = albumArtist,
									SongCount = songCount
								};
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

			return album;
		}
		public Album GetAlbum(Song song)
		{
			var album = new Album();

			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "SELECT * FROM Album WHERE Title=@Title";
					_logger.Info($"Song title to rerieve album:\n{song.AlbumTitle}");
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Title", song.AlbumTitle);

						using (var reader = cmd.ExecuteReader())
						{
							while (reader.Read())
							{
								var id = Convert.ToInt32(reader["AlbumId"]);
								var title = reader["Title"].ToString();
								var albumArtist = reader["AlbumArtist"].ToString();
								var songCount = Convert.ToInt32(reader["SongCount"]);
								album = new Album
								{
									AlbumId = id,
									Title = title,
									AlbumArtist = albumArtist,
									SongCount = songCount
								};
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

			return album;
		}

		public bool DoesAlbumExist(Album album)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Album WHERE AlbumId=@AlbumId";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@AlbumId", album.AlbumId);
						
						using (var reader = cmd.ExecuteReader())
						{
							album =  ParseSingleData(reader);

							if (album.Title != null)
							{
								_logger.Info($"Album {album.Title} exists");
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
		public bool DoesAlbumExist(Song song)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Album WHERE Title=@Title";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Title", song.AlbumTitle);
						
						using (var reader = cmd.ExecuteReader())
						{
							var album =  ParseSingleData(reader);

							if (!String.IsNullOrEmpty(album.Title))
							{
								_logger.Info($"Album {album.Title} exists");
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

		public void SaveAlbum(Album album)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "INSERT INTO Album(Title, AlbumArtist, " +
						"SongCount) VALUES (@Title, @AlbumArtist, " +
						"@SongCount)";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Title", album.Title);
						cmd.Parameters.AddWithValue("@AlbumArtist", album.AlbumArtist);
						cmd.Parameters.AddWithValue("@SongCount", album.SongCount);

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
		public void UpdateAlbum(Album album)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "UPDATE Album SET Title=@Title, AlbumArtist=" +
						"@AlbumArtist, SongCount=@SongCount WHERE AlbumId=@AlbumId";

					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Title", album.Title);
						cmd.Parameters.AddWithValue("@AlbumArtist", album.AlbumArtist);
						cmd.Parameters.AddWithValue("@SongCount", album.SongCount);
						cmd.Parameters.AddWithValue("@AlbumId", album.AlbumId);

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

		private List<Album> ParseData(MySqlDataReader reader)
		{
			List<Album> albums = new List<Album>();
			_logger.Info("Retrieving album records");

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["AlbumId"]);
				var title = reader["Title"].ToString();
				var albumArtist = reader["AlbumArtist"].ToString();
				var songCount = Convert.ToInt32(reader["SongCount"]);
				albums.Add(new Album
				{
					AlbumId = id,
					Title = title,
					AlbumArtist = albumArtist,
					SongCount = songCount
				});
			}

			_logger.Info("Album records retrieved");

			return albums;
		}
		private Album ParseSingleData(MySqlDataReader reader)
		{
			Album album = new Album();
			_logger.Info("Retrieving single album record");

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["AlbumId"]);
				var title = reader["Title"].ToString();
				var albumArtist = reader["AlbumArtist"].ToString();
				var songCount = Convert.ToInt32(reader["SongCount"]);
				album = new Album
				{
					AlbumId = id,
					Title = title,
					AlbumArtist = albumArtist,
					SongCount = songCount
				};
			}
			_logger.Info("Single ablum retreived");

			return album;
		}
		#endregion
	}
}
