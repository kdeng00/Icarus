using System;
using System.Collections.Generic;

using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Models.Context
{
	public class UserStoreContext: BaseStoreContext
	{
		#region Fields
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public UserStoreContext(string connectionString)
		{
			_connectionString = connectionString;
		}
		#endregion


		#region Methods
		public void SaveUser(User user)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					string query = "INSERT INTO Users(Username, Password, Salt, " +
									"Email, PhoneNumber, Firstname, Lastname, " +
									"DateCreated, LastLogin) VALUES(@Username, " +
									"@Password, @Salt, @Email, @PhoneNumber, " +
									"@Firstname, @Lastname, @DateCreated, @LastLogin)";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Username", user.Username);
						cmd.Parameters.AddWithValue("@Password", user.Password);
						cmd.Parameters.AddWithValue("@Salt", user.Salt);
						cmd.Parameters.AddWithValue("@Email", user.Email);
						cmd.Parameters.AddWithValue("@PhoneNumber", user.PhoneNumber);
						cmd.Parameters.AddWithValue("@Firstname", user.Firstname);
						cmd.Parameters.AddWithValue("@Lastname", user.Lastname);
						cmd.Parameters.AddWithValue("@DateCreated", DateTime.Now);
						cmd.Parameters.AddWithValue("@LastLogin", DateTime.Now);


						cmd.ExecuteNonQuery();
					}
				}
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred:\n{exMsg}");
			}
		}

		public User RetrieveUser(User user)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "SELECT * FROM Users WHERE Username=@Username";

					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Username", user.Username);

						using (var reader = cmd.ExecuteReader())
						{
							while (reader.Read())
							{
								user.Id = Convert.ToInt32(reader["Id"]);
								user.Email = reader["Email"].ToString();
								user.PhoneNumber = reader["PhoneNumber"].ToString();
								user.Firstname = reader["Firstname"].ToString();
								user.Lastname = reader["Lastname"].ToString();
								user.DateCreated = DateTime.ParseExact(reader["DateCreated"].ToString(), "yyyy-MM-dd HH:mm:ss", System.Globalization.CultureInfo.InvariantCulture);
								user.LastLogin = DateTime.ParseExact(reader["LastLogin"].ToString(), "yyyy-MM-dd HH:mm:ss", System.Globalization.CultureInfo.InvariantCulture);
							}
						}
					}
				}
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred:\n{exMsg}");
			}

			return null;
		}
		#endregion
	}
}
