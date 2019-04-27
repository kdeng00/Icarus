using System;
using System.Security.Cryptography;

using BCrypt.Net;
using Microsoft.AspNetCore.Cryptography.KeyDerivation;

using Icarus.Models;

namespace Icarus.Controllers.Utilities
{
	public class PasswordEncryption
	{
		#region Fields
		#endregion


		#region Properties
		#endregion


		#region Constructor
		#endregion


		#region Methods
		public string HashPassword(User user)
		{
			try
			{
				string hashedPassword = string.Empty;
				hashedPassword = BCrypt.Net.BCrypt.HashPassword(user.Password);

				return hashedPassword;

			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
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
