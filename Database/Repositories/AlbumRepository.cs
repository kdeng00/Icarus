using System;
using System.Collections.Generic;
using System.Globalization;

using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Database.Repositories
{
    public class AlbumRepository : BaseRepository
    {
	#region Fields
	#endregion


	#region Properties
	#endregion


	#region Constructors
	public AlbumRepository(string connectionString)
	{
	    _connectionString = connectionString;
	}
	#endregion


	#region Methods
	public List<Album> GetAlbumWithoutCount()
	{
	    var albums = new List<Album>();

	    if (AnyAlbums())
		try
	        {
		    using (MySqlConnection conn = GetConnection())
		    {
			conn.Open();
			var query = "SELECT * FROM Album";
			using (MySqlCommand cmd = new MySqlCommand(query, conn))
			    using (var reader = cmd.ExecuteReader())
				albums = ParseData(reader);
		    }
		}
		catch (Exception ex)
		{
		    var msg = ex.Message;
		    _logger.Error(msg, "An error occurred");
		}

	    return albums;
	}
        public List<Album> GetAlbums()
        {
            List<Album> albums = new List<Album>();

            try
            {
                using (MySqlConnection conn = GetConnection())
                {
                    conn.Open();
                    var query = "SELECT alb.*,COUNT(*) AS SongCount FROM Album alb " +
                        "LEFT JOIN Song sng ON alb.AlbumId=sng.AlbumId WHERE " +
                        "alb.AlbumId=sng.AlbumId GROUP BY alb.AlbumId";
                    using (MySqlCommand cmd = new MySqlCommand(query, conn))
                        using (var reader = cmd.ExecuteReader())
                            albums = ParseData(reader);
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }

            return albums;
        }

        public Album GetAlbum(Album album)
        {

            try
            {
                using (MySqlConnection conn = GetConnection())
                {
                    conn.Open();
                    var query = "SELECT alb.*, COUNT(*) AS SongCount FROM Album alb " + 
                        "LEFT JOIN Song sng ON alb.AlbumId=sng.AlbumId WHERE "+
                        "alb.AlbumId=@AlbumId LIMIT 1";
                    using (MySqlCommand cmd = new MySqlCommand(query, conn))
                    {
                        cmd.Parameters.AddWithValue("@AlbumId", album.AlbumId);

                        using (var reader = cmd.ExecuteReader())
                            album = ParseSingleData(reader);
                    }
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }

            return album;
        }
        public Album GetAlbum(Song song)
        {
            var album = new Album();

            try
            {
                using (MySqlConnection conn = GetConnection())
                {
                    conn.Open();
                    var query = "SELECT alb.*, 0 AS SongCount FROM Album alb " +
                        "WHERE alb.Title=@Title LIMIT 1";
                    
                    using (MySqlCommand cmd = new MySqlCommand(query, conn))
                    {
                        Console.WriteLine($"Album title {song.AlbumTitle}");
                        cmd.Parameters.AddWithValue("@Title", song.AlbumTitle);

                        using (var reader = cmd.ExecuteReader())
                            album = ParseSingleData(reader);
                    }
                }
            }
            catch (Exception ex)
            {
                var msg = ex.Message;
                _logger.Error(msg, "An error occurred");
            }

            return album;
	}
	public Album GetAlbum(Song song, bool retrieveCount)
	{
	    var album = new Album();

	    try
	    {
	        using (MySqlConnection conn = GetConnection())
		{
		    conn.Open();
		    var query = string.Empty;
		    if (retrieveCount)
		        query = "SELECT alb.*, COUNT(*) AS SongCount FROM Album alb " +
			    "LEFT JOIN Song sng ON alb.AlbumId=sng.AlbumId WHERE " +
			    "alb.Title=@Title GROUP BY alb.AlbumId LIMIT 1";
		    else
		        query = "SELECT alb.*, 0 AS SongCount FROM Album alb WHERE " +
			    "alb.Title=@Title LIMIT 1";
					
		    using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    {
			cmd.Parameters.AddWithValue("@Title", song.AlbumTitle);
            Console.WriteLine($"repo alb {song.AlbumTitle}");

			using (var reader = cmd.ExecuteReader())
			    album = ParseSingleData(reader);
		    }
	        }
	    }
	    catch (Exception ex)
	    {
		var msg = ex.Message;
	        _logger.Error(msg, "An error occurred");
	    }

	    return album;
	}

	public bool DoesAlbumExist(Album album)
	{
	    try
	    {
	        using (MySqlConnection conn = GetConnection())
		{
		    conn.Open();

 		    var query = "SELECT alb.*, 0 AS SongCount FROM Album alb WHERE alb.AlbumId=@AlbumId";
		    using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    {
		        cmd.Parameters.AddWithValue("@AlbumId", album.AlbumId);
						
			using (var reader = cmd.ExecuteReader())
			{
			    album =  ParseSingleData(reader);

			    if (album.Title != null)
			    {
			        _logger.Info($"Album {album.Title} exists");
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
	    return false;
	}
	public bool DoesAlbumExist(Song song)
	{
	    try
	    {
	        using (MySqlConnection conn = GetConnection())
		{
		    conn.Open();

		    var query = "SELECT alb.*, 0 AS SongCount FROM Album alb WHERE alb.Title=@Title";
		    using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    {
		        cmd.Parameters.AddWithValue("@Title", song.AlbumTitle);
					
			using (var reader = cmd.ExecuteReader())
			{
			    var album =  ParseSingleData(reader);

			    if (!String.IsNullOrEmpty(album.Title))
			    {
			        _logger.Info($"Album {album.Title} exists");
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
	    return false;
	}

	public void SaveAlbum(Album album)
	{
	    try
	    {
	        using (MySqlConnection conn = GetConnection())
		{
		    conn.Open();
		    var query = "INSERT INTO Album(Title, AlbumArtist)" +
		        " VALUES (@Title, @AlbumArtist)";
		    using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    {
		        cmd.Parameters.AddWithValue("@Title", album.Title);
			cmd.Parameters.AddWithValue("@AlbumArtist", album.AlbumArtist);

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
	public void UpdateAlbum(Album album)
	{
	    try
	    {
	        using (MySqlConnection conn = GetConnection())
		{
		    conn.Open();
		    var query = "UPDATE Album SET Title=@Title, AlbumArtist=" +
		        "@AlbumArtist WHERE AlbumId=@AlbumId";

		    using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    {
			cmd.Parameters.AddWithValue("@Title", album.Title);
			cmd.Parameters.AddWithValue("@AlbumArtist", album.AlbumArtist);
			cmd.Parameters.AddWithValue("@AlbumId", album.AlbumId);

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
	public void DeleteAlbum(Album album)
	{
	    try
	    {
		using (var conn = GetConnection())
		{
		    conn.Open();
		    var query = "DELETE FROM Album WHERE AlbumId=@AlbumId";

		    using (var cmd = new MySqlCommand(query, conn))
		    {
			cmd.Parameters.AddWithValue("@AlbumId", album.AlbumId);

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

	private List<Album> ParseData(MySqlDataReader reader)
	{
	    List<Album> albums = new List<Album>();
	    _logger.Info("Retrieving album records");

            while (reader.Read())
	    {
	        var id = Convert.ToInt32(reader["AlbumId"]);
	        var title = reader["Title"].ToString();
	        var albumArtist = reader["AlbumArtist"].ToString();
	        var songCount = Convert.ToInt32(reader["SongCount"]);
	        albums.Add(new Album
	        {
		    AlbumId = id,
		    Title = title,
		    AlbumArtist = albumArtist,
	            SongCount = songCount
	        });
	    }

	    _logger.Info("Album records retrieved");

	    return albums;
	}
	private Album ParseSingleData(MySqlDataReader reader)
	{
	    Album album = new Album();
	    _logger.Info("Retrieving single album record");

	    while (reader.Read())
	    {
		var id = Convert.ToInt32(reader["AlbumId"]);
		var title = reader["Title"].ToString();
		var albumArtist = reader["AlbumArtist"].ToString();
		var songCount = Convert.ToInt32(reader["SongCount"]);
		album = new Album
		{
		    AlbumId = id,
		    Title = title,
		    AlbumArtist = albumArtist,
		    SongCount = songCount
	        };
	    }
	    _logger.Info("Single ablum retreived");

	    return album;
	}

	private bool AnyAlbums()
	{
            var albums = new List<Album>();

	    using (var conn = GetConnection())
	    {
		conn.Open();

		var query = "SELECT * FROM Album";

		using (var cmd = new MySqlCommand(query, conn))
		    using (var reader = cmd.ExecuteReader())
		        albums = ParseData(reader);
	    }

	    if (albums.Count > 0)
	        return true;

	    return false;
	}
        #endregion
    }
}
