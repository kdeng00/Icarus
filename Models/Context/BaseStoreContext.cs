using System;

using MySql.Data.MySqlClient;
using NLog;

namespace Icarus.Models.Context
{
	public class BaseStoreContext
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
