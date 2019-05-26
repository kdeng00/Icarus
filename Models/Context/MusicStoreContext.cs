using System;    
using System.Collections.Generic;    

using MySql.Data.MySqlClient;    
        
namespace Icarus.Models.Context    
{    
	public class MusicStoreContext : BaseStoreContext
    	{    
		#region Fields
		#endregion


		#region Properties
		#endregion
    

		#region Constructors
        	public MusicStoreContext(string connectionString)    
        	{    
            		_connectionString = connectionString;    
        	}    
		#endregion


		#region Methods
		public void SaveSong(Song song)
		{
	    		try
	    		{
				_logger.Info("Saving song to the database");
	     	   		using (MySqlConnection conn = GetConnection())
				{
		    			conn.Open();
		    			string query = "INSERT INTO Song(Title, AlbumTitle, Artist," +
						" Year, Genre, Duration, Filename, SongPath, AlbumId, " +
						"ArtistId, GenreId, YearId) VALUES(@Title, @AlbumTitle, " +
						"@Artist, @Year, @Genre, @Duration, @Filename, @SongPath, " +
						"@AlbumId, @ArtistId, @GenreId, @YearId)";
		    			using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    			{
		        			cmd.Parameters.AddWithValue("@Title", song.Title);
						cmd.Parameters.AddWithValue("@AlbumTitle", song.AlbumTitle);
						cmd.Parameters.AddWithValue("@Artist", song.Artist);
						cmd.Parameters.AddWithValue("@Year", song.Year);
						cmd.Parameters.AddWithValue("@Genre", song.Genre);
						cmd.Parameters.AddWithValue("@Duration", song.Duration);
						cmd.Parameters.AddWithValue("@Filename", song.Filename);
						cmd.Parameters.AddWithValue("@SongPath", song.SongPath);
						cmd.Parameters.AddWithValue("@AlbumId", song.AlbumId);
						cmd.Parameters.AddWithValue("@ArtistId", song.ArtistId);
						cmd.Parameters.AddWithValue("@GenreId", song.GenreId);
						cmd.Parameters.AddWithValue("@YearId", song.YearId);

						cmd.ExecuteNonQuery();
		    			}
				}
				_logger.Info("Successfully saved song to the database");
	    		}
	    		catch(Exception ex)
	    		{
	        		var exMsg = ex.Message;
				Console.WriteLine($"An error occurred:\n{exMsg}");
				_logger.Error(exMsg, "An error occurred");
	    		}
		}
		public void UpdateSong(Song song)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();

		    			string query = "UPDATE Song SET Title=@Title, AlbumTitle=@AlbumTitle, " + 
						"Artist=@Artist, Year=@Year, Genre=@Genre, Duration=@Duration, " +
						"Filename=@Filename, SongPath=@SongPath, AlbumId=@AlbumId, " +
						"ArtistId=@ArtistId, GenreId=@GenreId, YearId=@YearId  WHERE Id=@Id";

					using (MySqlCommand cmd = new MySqlCommand(query, conn))
					{
		        			cmd.Parameters.AddWithValue("@Title", song.Title);
						cmd.Parameters.AddWithValue("@AlbumTitle", song.AlbumTitle);
						cmd.Parameters.AddWithValue("@Artist", song.Artist);
						cmd.Parameters.AddWithValue("@Year", song.Year);
						cmd.Parameters.AddWithValue("@Genre", song.Genre);
						cmd.Parameters.AddWithValue("@Duration", song.Duration);
						cmd.Parameters.AddWithValue("@Filename", song.Filename);
						cmd.Parameters.AddWithValue("@SongPath", song.SongPath);
						cmd.Parameters.AddWithValue("@Id", song.Id);
						cmd.Parameters.AddWithValue("@AlbumId", song.AlbumId);
						cmd.Parameters.AddWithValue("@ArtistId", song.ArtistId);
						cmd.Parameters.AddWithValue("@GenreId", song.GenreId);
						cmd.Parameters.AddWithValue("@YearId", song.YearId);

						cmd.ExecuteNonQuery();
					}
				}
				_logger.Info("Updated song");
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				Console.WriteLine("An error occurred in MusicStoreContext:");
				Console.WriteLine(msg);
				_logger.Error(msg, "An error occurred");
			}
		}
		public void DeleteSong(Song song)
		{
			try
			{
				_logger.Info("Deleting song record");

				using (var conn = GetConnection())
				{
					conn.Open();

					var query = "Delete FROM Song WHERE Id=@Id";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Id", song.Id);

						cmd.ExecuteNonQuery();
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}
		}
		public void DeleteSong(int id)
		{
	    		try
	    		{
				_logger.Info("Deleting song record");

	        		using (MySqlConnection conn = GetConnection())
				{
		    			conn.Open();
			    		string query = "DELETE FROM Song WHERE Id=@Id";
				   
			    		using (MySqlCommand cmd = new MySqlCommand(query, conn))
			    		{
		        			cmd.Parameters.AddWithValue("@Id", id);

						cmd.ExecuteNonQuery();
		    			}
				}
	    		}
	    		catch (Exception ex)
	    		{
	        		var exMsg = ex.Message;
				Console.WriteLine($"An error occurred:\n{exMsg}");
				_logger.Error(exMsg, "An error occurred");
	    		}
		}

		public List<Song> GetAllSongs()
		{
	    		List<Song> songs = new List<Song>();

	    		try
	    		{
				_logger.Info("Retrieving songs from the database");
	        		using (MySqlConnection conn = GetConnection())
				{
		    			conn.Open();
		    			var query = "SELECT * FROM Song";
					Console.WriteLine("ffff");
		    			MySqlCommand cmd = new MySqlCommand(query, conn);
		    			using (var reader = cmd.ExecuteReader())
		    			{
						songs = ParseData(reader);
		    			}
				}
	    		}
	    		catch (Exception ex)
	    		{
	        		var exMsg = ex.Message;
				Console.WriteLine($"An error ocurred:\n{exMsg}");
				songs.Clear();
				_logger.Error(exMsg, "An error occurred");
	    		}

	    		return songs;
		}

		public Song GetSong(Song song)
		{
			try
			{
				_logger.Info("Retrieving song from database");

				using (var conn = GetConnection())
				{
					conn.Open();
					var query = "SELECT * FROM Song WHERE Id=@Id";

					using (var cmd = new MySqlCommand(query, conn))
					{
						cmd.Parameters.AddWithValue("@Id", song.Id);

						using (var reader = cmd.ExecuteReader())
						{
							song = ParseSingleData(reader);
						}
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				_logger.Error(msg, "An error occurred");
			}

			return song;
		}
		public Song GetSong(int id)
		{
	    		Song song = new Song();

	    		try
	    		{
				_logger.Info("Retrieving song from database");
	        		using (MySqlConnection conn = GetConnection())
				{
		    			conn.Open();
		    			var query = "SELECT * FROM Song WHERE Id=@Id";

		    			MySqlCommand cmd = new MySqlCommand(query, conn);
		    			cmd.Parameters.AddWithValue("@Id", id);

		    			using (var reader = cmd.ExecuteReader())
		    			{
						song = ParseSingleData(reader);
		    			}
				}
				_logger.Info("Song found");
	    		}
	    		catch(Exception ex)
	    		{
	        		var exMsg = ex.Message;
				Console.WriteLine($"An error ocurred: {exMsg}");
				_logger.Error(exMsg, "An error occurred");
	    		}

	    		return song;
		}

		public bool DoesSongExist(Song song)
		{
			_logger.Info("Checking to see if the song exists");
			var songInDatabase = GetSong(song);
			var title = songInDatabase.Title;

			if (!string.IsNullOrEmpty(title))
			{
				_logger.Info("Song exists");
				return true;
			}

			_logger.Info("Song does not exists");

			return false;
		}
        
		private List<Song> ParseData(MySqlDataReader reader)
		{

			List<Song> songs = new List<Song>();
			while (reader.Read())
			{
		        	songs.Add(new Song
			    	{
			    		Id = Convert.ToInt32(reader["Id"]),
			        	Title = reader["Title"].ToString(),
			        	AlbumTitle = reader["AlbumTitle"].ToString(),
			        	Artist = reader["Artist"].ToString(),
			        	Year = Convert.ToInt32(reader["Year"]),
			        	Genre = reader["Genre"].ToString(),
			        	Duration = Convert.ToInt32(reader["Duration"]),
			        	Filename = reader["Filename"].ToString(),
			        	SongPath = reader["SongPath"].ToString(),
					AlbumId = Convert.ToInt32(reader["AlbumId"].ToString()),
					ArtistId = Convert.ToInt32(reader["ArtistId"].ToString()),
					GenreId = Convert.ToInt32(reader["GenreId"].ToString()),
					YearId = Convert.ToInt32(reader["YearId"].ToString())
			    	});
			}

			return songs;
		}

		private Song ParseSingleData(MySqlDataReader reader)
		{
			Song song = new Song();

			while (reader.Read())
			{
		        	song.Id = Convert.ToInt32(reader["Id"]);
				song.Title = reader["Title"].ToString();
				song.AlbumTitle = reader["AlbumTitle"].ToString();
				song.Artist = reader["Artist"].ToString();
				song.Year = Convert.ToInt32(reader["Year"].ToString());
				song.Genre = reader["Genre"].ToString();
				song.Duration = Convert.ToInt32(reader["Duration"]);
				song.Filename = reader["Filename"].ToString();
				song.SongPath = reader["SongPath"].ToString();
				song.AlbumId = Convert.ToInt32(reader["AlbumId"].ToString());
				song.ArtistId = Convert.ToInt32(reader["ArtistId"].ToString());
				song.GenreId = Convert.ToInt32(reader["GenreId"].ToString());
				song.YearId = Convert.ToInt32(reader["YearId"].ToString());
			}

			return song;
		}

		#endregion
    	}    
}  
