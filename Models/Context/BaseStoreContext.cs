using System;

using MySql.Data.MySqlClient;

namespace Icarus.Models.Context
{
	public class BaseStoreContext
	{
		#region Fields
		protected string _connectionString;
		#endregion


		#region Methods
		protected MySqlConnection GetConnection()
		{
			return new MySqlConnection(_connectionString);
		}
		#endregion
	}
}
