using System;
using System.Collections.Generic;
using System.Globalization;

using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Database.Repositories
{
	public class YearRepository : BaseRepository
	{
		#region Fields
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public YearRepository(string connectionString)
		{
			_connectionString = connectionString;
		}
		#endregion


		#region Methods
		public List<Year> GetSongYears()
		{
			_logger.Info("Retrieving Year records");

			var yearValues = new List<Year>();

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT yr.*, COUNT(*) AS SongCount FROM Year " +
						"yr LEFT JOIN Song sng ON yr.YearId=sng.YearId " +
						"GROUP BY yr.YearId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						using (var reader = cmd.ExecuteReader())
						{
							yearValues = ParseData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return yearValues;
		}

		public Year GetSongYear(Year year)
		{
			_logger.Info("Retrieving Year record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT yr.*, COUNT(*) AS SongCount FROM Year " +
						"yr LEFT JOIN Song sng ON yr.YearId=sng.YearId WHERE " +
						"YearId=@YearId GROUP BY yr.YearId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearId", year.YearId);

						using (var reader = cmd.ExecuteReader())
						{
							year = ParseSingleData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return year;
		}
		public Year GetSongYear(Song song)
		{
			var year = new Year();

			_logger.Info("Retrieving Year record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT yr.*, 0 AS SongCount FROM Year " +
						"yr WHERE yr.YearValue=@YearValue";

					using(var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearValue", song.Year);

						using (var reader = cmd.ExecuteReader())
						{
							year = ParseSingleData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return year;
		}
		public Year GetSongYear(Song song, bool retrieveCount)
		{
			var year = new Year();

			_logger.Info("Retrieving year record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = string.Empty;

					if (retrieveCount)
					{
						query = "SELECT yr.*, COUNT(*) AS SongCount FROM Year yr " +
							"LEFT JOIN Song sng ON yr.YearValue=sng.Year WHERE " +
							"yr.YearValue=@YearValue GROUP BY yr.YearId LIMIT 1";
					}
					else
					{
						query = "SELECT yr.*, 0 AS SongCount FROM Year yr " +
							"WHERE yr.YearValue=@YearValue LIMIT 1";
					}

					using(var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearValue", song.Year);

						using (var reader = cmd.ExecuteReader())
						{
							year = ParseSingleData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return year;
		}

		public bool DoesYearExist(Year year)
		{
			_logger.Info("Checking to see if Year record exists");
			
			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT yr.*, 0 AS SongCount FROM Year yr WHERE " +
						"yr.YearId=@YearId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearId", year.YearId);

						using (var reader = cmd.ExecuteReader())
						{
							year = ParseSingleData(reader);

							if (year.YearValue > 0)
							{
								_logger.Info("Year record exists");

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

			_logger.Info("Year record does not exist");

			return false;
		}
		public bool DoesYearExist(Song song)
		{
			_logger.Info("Checking to see if Year record exists");
			
			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "SELECT yr.*, 0 AS SongCount FROM Year yr WHERE " +
						"yr.YearValue=@YearValue";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearValue", song.Year);

						using (var reader = cmd.ExecuteReader())
						{
							var year = ParseSingleData(reader);

							if (year.YearValue > 0)
							{
								_logger.Info("Year record exists");

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

			_logger.Info("Year record does not exist");

			return false;
		}

		public void SaveYear(Year year)
		{
			_logger.Info("Saving Year record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "INSERT INTO Year(YearValue) VALUES(@YearValue)";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearValue", year.YearValue);

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
		public void UpdateYear(Year year)
		{
			_logger.Info("Deleting Year record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "UPDATE Year SET YearValue=@YearValue WHERE YearId=@YearId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearId", year.YearId);
						cmd.Parameters.AddWithValue("@YearValue", year.YearValue);

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
		public void DeleteYear(Year year)
		{
			_logger.Info("Deleting Year record");

			try
			{
				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "DELETE FROM Year WHERE YearId=@YearId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearId", year.YearId);

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

		private List<Year> ParseData(MySqlDataReader reader)
		{
			var yearValues = new List<Year>();

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["YearId"].ToString());
				var year = Convert.ToInt32(reader["YearValue"].ToString());
				var songCount = Convert.ToInt32(reader["SongCount"].ToString());

				yearValues.Add(new Year
				{
					YearId = id,
					YearValue = year,
					SongCount = songCount
				});
			}


			return yearValues;
		}

		private Year ParseSingleData(MySqlDataReader reader)
		{
			var yearValue = new Year();

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["YearId"].ToString());
				var year = Convert.ToInt32(reader["YearValue"].ToString());
				var songCount = Convert.ToInt32(reader["SongCount"].ToString());

				yearValue.YearId = id;
				yearValue.YearValue = year;
				yearValue.SongCount = songCount;
			}

			return yearValue;
		}
		#endregion
	}
}
