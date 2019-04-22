using System;    
using System.Collections.Generic;    

using MySql.Data.MySqlClient;    
        
namespace Icarus.Models.Context    
{    
    public class MusicStoreContext    
    {    
        public string ConnectionString { get; set; }    
    
        public MusicStoreContext(string connectionString)    
        {    
            this.ConnectionString = connectionString;    
        }    


		public void SaveSong(Song song)
		{
	    	try
	    	{
	     	   using (MySqlConnection conn = GetConnection())
				{
		    		conn.Open();
		    		string query = "INSERT INTO Songs(Title, Album, Artist," +
						" Year, Genre, Duration, Filename, SongPath) " + 
						"VALUES(@Title, @Album, @Artist, @Year, @Genre, " +
				   		"@Duration, @Filename, @SongPath)";
		    		using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    		{
		        		cmd.Parameters.AddWithValue("@Title", song.Title);
						cmd.Parameters.AddWithValue("@Album", song.Album);
						cmd.Parameters.AddWithValue("@Artist", song.Artist);
						cmd.Parameters.AddWithValue("@Year", song.Year);
						cmd.Parameters.AddWithValue("@Genre", song.Genre);
						cmd.Parameters.AddWithValue("@Duration", song.Duration);
						cmd.Parameters.AddWithValue("@Filename", song.Filename);
						cmd.Parameters.AddWithValue("@SongPath", song.SongPath);

						cmd.ExecuteNonQuery();
		    		}
				}
	    	}
	    	catch(Exception ex)
	    	{
	        	var exMsg = ex.Message;
				Console.WriteLine($"An error occurred:\n{exMsg}");
	    	}
		}
		public void DeleteSong(int id)
		{
	    	try
	    	{
	        	using (MySqlConnection conn = GetConnection())
				{
		    		conn.Open();
		    		string query = "DELETE FROM Songs WHERE Id=@Id";
				   
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
		    		var query = "SELECT * FROM Songs";
		    		MySqlCommand cmd = new MySqlCommand(query, conn);
		    		using (var reader = cmd.ExecuteReader())
		    		{
						while (reader.Read())
						{
		            		songs.Add(new Song
			    			{
			        			Id = Convert.ToInt32(reader["Id"]),
			        			Title = reader["Title"].ToString(),
			        			Album = reader["Album"].ToString(),
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
		    		var query = "SELECT * FROM Songs WHERE Id=@Id";

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
			        			Album = reader["Album"].ToString(),
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
	    	}
	    	catch(Exception ex)
	    	{
	        	var exMsg = ex.Message;
				Console.WriteLine($"An error ocurred: {exMsg}");
	    	}

	    	return song;
		}
        
        private MySqlConnection GetConnection()    
        {    
            return new MySqlConnection(ConnectionString);    
        }  
    }    
}  
