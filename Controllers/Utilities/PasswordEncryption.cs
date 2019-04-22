using System;
using System.Security.Cryptography;

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
		public User HashPassword(User user)
		{
			try
			{
				var userSalt = GenerateSalt();
				var userHash = GenerateHash(user.Password, userSalt);

				user.Password = userHash;
				user.Salt = userSalt;

				return user;
			}
			catch (Exception ex)
			{
				var exMsg = ex.Message;
				Console.WriteLine($"An error occurred {exMsg}");
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
