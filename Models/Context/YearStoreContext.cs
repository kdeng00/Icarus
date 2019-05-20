using System;
using System.Collections.Generic;
using System.Globalization;

using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Models.Context
{
	// TODO: Implement Year store #42
	public class YearStoreContext : BaseStoreContext
	{
		#region Fields
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public YearStoreContext(string connectionString)
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

					var query = "SELECT * FROM Year";

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

					var query = "SELECT * FROM Year WHERE YearId=@YearId";

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

					var query = "SELECT * FROM Year WHERE YearValue=@YearValue";

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

					var query = "SELECT * FROM Year WHERE YearId=@YearId";

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

					var query = "SELECT * FROM Year WHERE YearValue=@YearValue";

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

					var query = "INSERT INTO Year(YearValue, SongCount) VALUES(" +
						"@YearValue, @SongCount)";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearValue", year.YearValue);
						cmd.Parameters.AddWithValue("@SongCount", year.SongCount);

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

					var query = "UPDATE Year SET YearValue=@YearValue, SongCount=@SongCount " +
						"WHERE YearId=@YearId";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@YearId", year.YearId);
						cmd.Parameters.AddWithValue("@YearValue", year.YearValue);
						cmd.Parameters.AddWithValue("@SongCount", year.SongCount);

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
