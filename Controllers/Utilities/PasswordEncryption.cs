using System;
using System.Security.Cryptography;

using BCrypt.Net;
using Microsoft.AspNetCore.Cryptography.KeyDerivation;
using NLog;

using Icarus.Models;

namespace Icarus.Controllers.Utilities
{
	public class PasswordEncryption
	{
		#region Fields
		private static Logger _logger = NLog.LogManager.GetCurrentClassLogger();
		#endregion


		#region Properties
		#endregion


		#region Constructor
		#endregion


		#region Methods
		public bool VerifyPassword(User user, string password)
		{
			try
			{
				var result = BCrypt.Net.BCrypt.Verify(password, user.Password);

				return result;
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return false;
		}

		public string HashPassword(User user)
		{
			try
			{
				string hashedPassword = string.Empty;
				hashedPassword = BCrypt.Net.BCrypt.HashPassword(user.Password);

				_logger.Info("Successfully hashed password");

				return hashedPassword;

			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				_logger.Error(exMsg, "An error occurred");
			}

			return null;
		}

		string GenerateHash(string password, byte[] salt)
		{

			string hashed = Convert.ToBase64String(KeyDerivation.Pbkdf2(
						password: password,
						salt: salt,
						prf: KeyDerivationPrf.HMACSHA1,
						iterationCount: 10000,
						numBytesRequested: 256/8));

			return hashed;
		}

		byte[] GenerateSalt()
		{
			byte[] salt = new byte[128/8];

			using (var rng = RandomNumberGenerator.Create())
			{
				rng.GetBytes(salt);
			}


			return salt;
		}
		#endregion
	}
}
