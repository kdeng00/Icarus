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
							while (reader.Read())
							{
								var id = Convert.ToInt32(reader["Id"].ToString());
								var name = reader["Name"].ToString();
								var songCount = Convert.ToInt32(reader["SongCount"].ToString());
								artists.Add(new Artist
								{
									Id = id,
									Name = name,
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

			return artists;
		}
		#endregion
	}
}
