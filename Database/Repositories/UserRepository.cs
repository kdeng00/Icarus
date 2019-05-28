using System;
using System.Collections.Generic;
using System.Globalization;

using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Database.Repositories
{
	public class UserRepository: BaseRepository
	{
		#region Fields
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public UserRepository(string connectionString)
		{
			_connectionString = connectionString;
		}
		#endregion


		#region Methods
		public void SaveUser(User user)
		{
			try
			{
				_logger.Info("Saving user");

				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					string query = "INSERT INTO User(Username, Password, Nickname, Email" +
						", PhoneNumber, Firstname, Lastname, EmailVerified) " +
						"VALUES(@Username, @Password, @Nickname, @Email, @PhoneNumber," +
						" @Firstname, @Lastname, @EmailVerified)";

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


						cmd.ExecuteNonQuery();
					}
				}

				_logger.Info("Successfully saved user");
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred:\n{exMsg}");
				_logger.Error(exMsg, "An error occurred");
			}
		}

		public User RetrieveUser(User user)
		{
			try
			{
				_logger.Info("Retrieving user");

				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();
					var query = "SELECT * FROM User WHERE Username=@Username";

					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Username", user.Username);

						using (var reader = cmd.ExecuteReader())
						{
							user = ParseSingleData(reader);
						}
					}
				}

				_logger.Info("Successfully retrieved user");

				return user;
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred:\n{exMsg}");
				_logger.Error(exMsg, "An error occurred");
			}

			return null;
		}

		public bool DoesUserExist(User user)
		{
			var username = user.Username;
			try
			{
				_logger.Info($"Checking to see if {user.Username} exists");

				using (var conn = GetConnection())
				{
					conn.Open();
					var query = "SELECT * FROM User WHERE Username=@Username";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Username", user.Username);
						
						using (var reader = cmd.ExecuteReader())
						{
							user = ParseSingleData(reader, true);
							username = user.Username;

							if (!string.IsNullOrEmpty(username))
							{
								_logger.Info($"The user {username} exists");

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

			_logger.Info($"The user {username} does not exists");

			return false;
		}

		private User ParseSingleData(MySqlDataReader reader)
		{
			var user = new User();

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["Id"].ToString());
				var username = reader["Username"].ToString();
				var password = reader["Password"].ToString();
				var nickname = reader["Nickname"].ToString();
				var email = reader["Email"].ToString();
				var phoneNumber = reader["PhoneNumber"].ToString();
				var emailVerified = reader["EmailVerified"].ToString() == "1";
				var firstname = reader["Firstname"].ToString();
				var lastname = reader["Lastname"].ToString();
				var rawLastLogin = reader["LastLogin"].ToString();

				var parsedDateCreated = DateTime.Parse(reader["DateCreated"].ToString());

				var dateCreated = DateTime.Parse(parsedDateCreated.ToString("yyyy-MM-dd HH:mm:ss"));

				if (!string.IsNullOrEmpty(rawLastLogin))
				{
					var parsedLastLogin = DateTime.Parse(rawLastLogin);
					var lastLogin = DateTime.Parse(parsedLastLogin.ToString("yyyy-MM-dd HH:mm:ss"));
					user.LastLogin = lastLogin;
				}

				user.Id = id;
				user.Username = username;
				user.Password = password;
				user.Nickname = nickname;
				user.Email = email;
				user.PhoneNumber = phoneNumber;
				user.EmailVerified = emailVerified;
				user.Firstname = firstname;
				user.Lastname = lastname;
				user.DateCreated = dateCreated;
			}

			return user;
		}
		private User ParseSingleData(MySqlDataReader reader, bool ignoreLastLogin)
		{
			var user = new User();

			while (reader.Read())
			{
				var id = Convert.ToInt32(reader["Id"].ToString());
				var username = reader["Username"].ToString();
				var nickname = reader["Nickname"].ToString();
				var email = reader["Email"].ToString();
				var phoneNumber = reader["PhoneNumber"].ToString();
				var emailVerified = reader["EmailVerified"].ToString() == "1";
				var firstname = reader["Firstname"].ToString();
				var lastname = reader["Lastname"].ToString();

				var parsedDateCreated = DateTime.Parse(reader["DateCreated"].ToString());

				var dateCreated = DateTime.Parse(parsedDateCreated.ToString("yyyy-MM-dd HH:mm:ss"));

				if (!ignoreLastLogin)
				{
					var parsedLastLogin = DateTime.Parse(reader["LastLogin"].ToString());
					var lastLogin = DateTime.Parse(parsedLastLogin.ToString("yyyy-MM-dd HH:mm:ss"));
					user.LastLogin = lastLogin;
				}

				user.Id = id;
				user.Username = username;
				user.Nickname = nickname;
				user.Email = email;
				user.PhoneNumber = phoneNumber;
				user.EmailVerified = emailVerified;
				user.Firstname = firstname;
				user.Lastname = lastname;
				user.DateCreated = dateCreated;
			}

			return user;
		}
		#endregion
	}
}
