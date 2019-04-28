using System;
using System.Collections.Generic;
using System.Globalization;

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
					string query = "INSERT INTO Users(Username, Password, Nickname," +
									"Email, PhoneNumber, Firstname, Lastname, " +
									"EmailVerified, DateCreated, LastLogin) " +
									"VALUES(@Username, @Password, @Nickname, " + 
									"@Email, @PhoneNumber, @Firstname, @Lastname," + 
									" @EmailVerified, @DateCreated, @LastLogin)";
					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Username", user.Username);
						cmd.Parameters.AddWithValue("@Password", user.Password);
						cmd.Parameters.AddWithValue("@Nickname", user.Nickname);
						cmd.Parameters.AddWithValue("@Email", user.Email);
						cmd.Parameters.AddWithValue("@PhoneNumber", user.PhoneNumber);
						cmd.Parameters.AddWithValue("@Firstname", user.Firstname);
						cmd.Parameters.AddWithValue("@Lastname", user.Lastname);
						cmd.Parameters.AddWithValue("@EmailVerified", user.EmailVerified);
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
								var dateCreated = reader["DateCreated"].ToString();
								var lastLogin = reader["LastLogin"].ToString();
								var parsedC = DateTime.Parse(dateCreated);
								var parsedL = DateTime.Parse(lastLogin);

								user.Id = Convert.ToInt32(reader["Id"]);
								user.Nickname = reader["Nickname"].ToString();
								user.Email = reader["Email"].ToString();
								user.PhoneNumber = reader["PhoneNumber"].ToString();
								user.EmailVerified = (reader["EmailVerified"].ToString()) == "1";
								user.Firstname = reader["Firstname"].ToString();
								user.Lastname = reader["Lastname"].ToString();
								user.DateCreated = DateTime.Parse(parsedC.ToString("yyyy-MM-dd HH:mm:ss"));
								user.LastLogin = DateTime.Parse(parsedL.ToString("yyyy-MM-dd HH:mm:ss"));
							}
						}
					}
				}
				return user;
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
