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
		public List<Year> GetSongYear()
		{
			var yearValues = new List<Year>();

			using (var conn = GetConnection())
			{
				conn.Open();

				var query = "SELECT * FROM Year";
				// TODO: Left off here for the YearStore
			}

			return yearValues;
		}
		#endregion
	}
}
