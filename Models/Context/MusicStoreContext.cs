using System;    
using System.Collections.Generic;    

using MySql.Data.MySqlClient;    
using NLog;
        
namespace Icarus.Models.Context    
{    
	public class MusicStoreContext : BaseStoreContext
    	{    
		#region Fields
		private static Logger _logger = NLog.LogManager.GetCurrentClassLogger();
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
	     	   		using (MySqlConnection conn = GetConnection())
				{
		    			conn.Open();
		    			string query = "INSERT INTO Song(Title, AlbumTitle, Artist," +
						" Year, Genre, Duration, Filename, SongPath, AlbumId) " + 
						"VALUES(@Title, @AlbumTitle, @Artist, @Year, @Genre, " +
				   		"@Duration, @Filename, @SongPath, @AlbumId)";
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

						cmd.ExecuteNonQuery();
		    			}
				}
	    		}
	    		catch(Exception ex)
	    		{
	        		var exMsg = ex.Message;
				Console.WriteLine($"An error occurred:\n{exMsg}");
				_logger.Error(exMsg, "An error occurred");
	    		}
		}
		// TODO: Update this method to be compatible with the new AlbumId field
		public void UpdateSong(Song song)
		{
			try
			{
				using (MySqlConnection conn = GetConnection())
				{
					conn.Open();

		    			string query = "UPDATE Song SET Title=@Title, AlbumTitle=@AlbumTitle, " +
							"Artist=@Artist, Year=@Year, Genre=@Genre, " +
				   			"Duration=@Duration, Filename=@Filename, " +
							"SongPath=@SongPath WHERE Id=@Id";
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
						cmd.Parameters.AddWithValue(@"Id", song.Id);

						cmd.ExecuteNonQuery();
					}
				}
			}
			catch (Exception ex)
			{
				var msg = ex.Message;
				Console.WriteLine("An error occurred in MusicStoreContext:");
				Console.WriteLine(msg);
				_logger.Error(msg, "An error occurred");
			}
		}
		public void DeleteSong(int id)
		{
	    		try
	    		{
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
	        		using (MySqlConnection conn = GetConnection())
				{
		    			conn.Open();
		    			var query = "SELECT * FROM Song";
		    			MySqlCommand cmd = new MySqlCommand(query, conn);
		    			using (var reader = cmd.ExecuteReader())
		    			{
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
			        				SongPath = reader["SongPath"].ToString()
			    				});
						}
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

		public Song GetSong(int id)
		{
	    		Song song = new Song();

	    		try
	    		{
	        		using (MySqlConnection conn = GetConnection())
				{
		    			conn.Open();
		    			var query = "SELECT * FROM Song WHERE Id=@Id";

		    			MySqlCommand cmd = new MySqlCommand(query, conn);
		    			cmd.Parameters.AddWithValue("@Id", id);

		    			using (var reader = cmd.ExecuteReader())
		    			{
						while (reader.Read())
						{
		            				song = new Song
			    				{
			        				Id = Convert.ToInt32(reader["Id"]),
			        				Title = reader["Title"].ToString(),
			        				AlbumTitle = reader["AlbumTitle"].ToString(),
			        				Artist = reader["Artist"].ToString(),
			        				Year = Convert.ToInt32(reader["Year"]),
			        				Genre = reader["Genre"].ToString(),
			        				Duration = Convert.ToInt32(reader["Duration"]),
			        				Filename = reader["Filename"].ToString(),
			        				SongPath = reader["SongPath"].ToString()
			    				};
						}
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
			        	SongPath = reader["SongPath"].ToString()
			    	});
			}

			return songs;
		}

		private Song ParseSingleData(MySqlDataReader reader)
		{
			Song song = new Song();


			while (reader.Read())
			{
		        	song = new Song
			    	{
			    		Id = Convert.ToInt32(reader["Id"]),
			        	Title = reader["Title"].ToString(),
			        	AlbumTitle = reader["AlbumTitle"].ToString(),
			        	Artist = reader["Artist"].ToString(),
			        	Year = Convert.ToInt32(reader["Year"]),
			        	Genre = reader["Genre"].ToString(),
			        	Duration = Convert.ToInt32(reader["Duration"]),
			        	Filename = reader["Filename"].ToString(),
			        	SongPath = reader["SongPath"].ToString()
			    	};
			}

			return song;
		}

		#endregion
    	}    
}  
