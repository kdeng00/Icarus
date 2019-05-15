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
