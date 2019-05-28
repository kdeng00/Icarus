using System;

using MySql.Data.MySqlClient;
using NLog;

namespace Icarus.Database.Repositories
{
	public class BaseRepository
	{
		#region Fields
		protected string _connectionString;
		protected static Logger _logger = NLog.LogManager.GetCurrentClassLogger();
		#endregion


		#region Methods
		protected MySqlConnection GetConnection()
		{
			return new MySqlConnection(_connectionString);
		}
		#endregion
	}
}
