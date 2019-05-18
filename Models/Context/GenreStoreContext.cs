using System;
using System.Collections.Generic;
using System.Globalization;

using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Models.Context
{
	// TODO: Implement Genre store #41
	public class GenreStoreContext : BaseStoreContext
	{
		#region Fields
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public GenreStoreContext(string connectionString)
		{
			_connectionString = connectionString;
		}
		#endregion


		#region Methods
		public List<Genre> GetGenres()
		{
			_logger.Info("Retrieving Genre records");

			var genres = new List<Genre>();

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Genre";

					using (var cmd = new MySqlCommand(query, conn))
					{
						using (var reader = cmd.ExecuteReader())
						{
							genres = ParseData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return genres;
		}

		public Genre GetGenre(Genre genre)
		{
			_logger.Info("Retrieving Genre record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Genre WHERE GenreId=@GenreId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@GenreId", genre.GenreId);

						using (var reader = cmd.ExecuteReader())
						{
							genre = ParseSingleData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return genre;
		}
		public Genre GetGenre(Song song)
		{
			_logger.Info("Retrieving Genre record");

			var genre = new Genre();

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Genre WHERE GenreName=@GenreName";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@GenreName", song.Genre);

						using (var reader = cmd.ExecuteReader())
						{
							genre = ParseSingleData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return genre;
		}

		public bool DoesGenreExist(Genre genre)
		{
			_logger.Info("Checking to see if Genre record exists");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Genre WHERE GenreId=@GenreId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@GenreId", genre.GenreId);

						using (var reader = cmd.ExecuteReader())
						{
							genre = ParseSingleData(reader);
							var genreName = genre.GenreName;

							if (!string.IsNullOrEmpty(genreName))
							{
								_logger.Info("Genre exists");

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

			_logger.Info("Genre does not exist");

			return false;
		}
		public bool DoesGenreExist(Song song)
		{
			_logger.Info("Checking to see if Genre record exists");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT * FROM Genre WHERE GenreName=@GenreName";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@GenreName", song.Genre);

						using (var reader = cmd.ExecuteReader())
						{
							var genre = ParseSingleData(reader);
							var genreName = genre.GenreName;

							if (!string.IsNullOrEmpty(genreName))
							{
								_logger.Info("Genre exists");

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

			_logger.Info("Genre does not exist");

			return false;
		}

		public void SaveGenre(Genre genre)
		{
			_logger.Info("Saving Genre record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "INSERT INTO Genre(GenreName, SongCount) VALUES(" +
						"@GenreName, @SongCount)";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@GenreName", genre.GenreName);
						cmd.Parameters.AddWithValue("@SongCount", genre.SongCount);

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
		public void UpdateGenre(Genre genre)
		{
			_logger.Info("Updating Genre record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "UPDATE Genre SET GenreName=@GenreName, " +
						"SongCount=@SongCount WHERE GenreId=@GenreId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@GenreName", genre.GenreName);
						cmd.Parameters.AddWithValue("@SongCount", genre.SongCount);
						cmd.Parameters.AddWithValue("@GenreId", genre.GenreId);

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
		public void DeleteGenre(Genre genre)
		{
			_logger.Info("Deleting Genre record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "DELETE Genre WHERE GenreId=@GenreId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@GenreId", genre.GenreId);

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

		private List<Genre> ParseData(MySqlDataReader reader)
		{
			var genres = new List<Genre>();
			_logger.Info("Retrieving genre records");

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["GenreId"].ToString());
				var genreName = reader["GenreName"].ToString();
				var songCount = Convert.ToInt32(reader["SongCount"].ToString());

				genres.Add(new Genre
				{
					GenreId = id,
					GenreName = genreName,
					SongCount = songCount
				});
			}

			_logger.Info("Genre records retrieved");

			return genres;
		}

		private Genre ParseSingleData(MySqlDataReader reader)
		{
			var genre = new Genre();
			_logger.Info("Retrieving single genre record");

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["GenreId"].ToString());
				var genreName = reader["GenreName"].ToString();
				var songCount = Convert.ToInt32(reader["SongCount"].ToString());
				genre.GenreId = id;
				genre.GenreName = genreName;
				genre.SongCount = songCount;
			}

			_logger.Info("Single genre record retrieved");

			return genre;
		}
		#endregion
	}
}
