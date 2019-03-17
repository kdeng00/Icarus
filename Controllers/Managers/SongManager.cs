using System;
using System.Collections.Generic;
using System.Configuration;

using Microsoft.Extensions.Configuration;

using MySql.Data;
using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Controllers.Managers
{
	public class SongManager
	{
		#region Fields
		private List<Song> _songs;
		private Song _song;
		private IConfiguration _config;
		private string _connectionString;
		#endregion


		#region Properties
		#endregion


		#region Constructors
		public SongManager()
		{
			Initialize();
		}

		public SongManager(Song song)
		{
			Initialize();
			_song = song;
		}
		public SongManager(IConfiguration config)
		{
			Initialize();
			_config = config;
		}
		#endregion


		#region Methods
		public void SaveSong(Song song)
		{
			_song = song;

		}

		public Song RetrieveSong(int id)
		{
			return new Song();
		}


		void Initialize()
		{
			try
			{
				_connectionString = _config.GetConnectionString("IcarusDev");
				Console.WriteLine(_connectionString);
			}
			catch (Exception ex)
			{
				Console.WriteLine($"Error Occurred: {ex.Message}");
			}
			
		}
		#endregion	
	}
}
