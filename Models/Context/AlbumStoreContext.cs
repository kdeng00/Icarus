using System;
using System.Collections.Generic;
using System.Globalization;

using MySql.Data.MySqlClient;
using NLog;

using Icarus.Models;

namespace Icarus.Models.Context
{
	public class AlbumStoreContext : BaseStoreContext
	{
		#region Fields
		private static Logger _logger = NLog.LogManager.GetCurrentClassLogger();
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
								var id = Convert.ToInt32(reader["Id"]);
								var title = reader["Title"].ToString();
								var albumArtist = reader["AlbumArtist"].ToString();
								var songCount = Convert.ToInt32(reader["SongCount"]);
								albums.Add(new Album
								{
									Id = id,
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
		#endregion
	}
}
