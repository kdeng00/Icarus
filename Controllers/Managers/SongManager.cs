using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.IO;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Configuration;

using Id3;
using Id3.Frames;
using MySql.Data;
using MySql.Data.MySqlClient;
using TagLib;

using Icarus.Models;
using Icarus.Controllers.Utilities;

namespace Icarus.Controllers.Managers
{
    public class SongManager
    {
        #region Fields
        private MySqlConnection _conn;
        private MySqlCommand _cmd;
        private MySqlDataAdapter _dataDump;
        private DataTable _results;
	private List<Song> _songs;
	private Song _song;
	private IConfiguration _config;
	private string _connectionString;
	private string _tempDirectoryRoot;
	private string _archiveDirectoryRoot;
	private string _compressedSongFilename;
	#endregion


	#region Properties
	public Song SongDetails
	{
	    get => _song;
	    set => _song = value;
	}

	public string ArchiveDirectoryRoot
	{
	    get => _archiveDirectoryRoot;
	    set => _archiveDirectoryRoot = value;
	}
	public string CompressedSongFilename
	{
	    get => _compressedSongFilename;
	    set => _compressedSongFilename = value;
	}
	#endregion


	#region Constructors
	public SongManager()
	{
	    Initialize();
            InitializeConnection();
	}

	public SongManager(Song song)
	{
	    Initialize();
            InitializeConnection();
			_song = song;
	}
	public SongManager(IConfiguration config)
	{
	    _config = config;
	    Initialize();
            InitializeConnection();
	}
	public SongManager(IConfiguration config, string tempDirectoryRoot)
	{
	    _config = config;
	    _tempDirectoryRoot = tempDirectoryRoot;
	    Initialize();
            InitializeConnection();
	}
	#endregion


	#region Methods
	public void SaveSongDetails()
	{
	    try
	    {
	        using (MySqlConnection conn = new MySqlConnection(_connectionString))
		{
		    conn.Open();
		    string query = "INSERT INTO Songs(Title, Album, Artist, Year, Genre, Duration, " +
				   "Filename, SongPath) VALUES(@Title, @Album, @Artist, @Year, @Genre, " +
				   "@Duration, @Filename, @SongPath)";
		    using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    {
		        cmd.Parameters.AddWithValue("@Title", _song.Title);
			cmd.Parameters.AddWithValue("@Album", _song.Album);
			cmd.Parameters.AddWithValue("@Artist", _song.Artist);
			cmd.Parameters.AddWithValue("@Year", _song.Year);
			cmd.Parameters.AddWithValue("@Genre", _song.Genre);
			cmd.Parameters.AddWithValue("@Duration", _song.Duration);
			cmd.Parameters.AddWithValue("@Filename", _song.Filename);
			cmd.Parameters.AddWithValue("@SongPath", _song.SongPath);

			cmd.ExecuteNonQuery();
		    }
		}
	    }
	    catch (Exception ex)
	    {
	        var exMsg = ex.Message;
		Console.WriteLine($"An Error Occurred: {exMsg}");
	    }
	}
	public void SaveSongDetails(Song song)
	{
	    try
	    {
	        using (MySqlConnection conn = new MySqlConnection(_connectionString))
		{
		    conn.Open();
		    string query = "INSERT INTO Songs(Title, Album, Artist, Year, Genre, Duration, " +
				   ", Filename, SongPath) VALUES(@Title, @Album, @Artist, @Year, @Genre, " +
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
	    catch (Exception ex)
	    {
	        var exMsg = ex.Message;
		Console.WriteLine($"An Error Occurred: {exMsg}");
	    }
	}

	public async Task SaveSong(SongData songData)
	{
	    try
	    {
		using (MySqlConnection conn = new MySqlConnection(_connectionString))
		{
		    conn.Open();
		    string query = "INSERT INTO SongData(Data) VALUES(@Data)";
		    using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    {
		        cmd.Parameters.AddWithValue("@Data", songData.Data);

			cmd.ExecuteNonQuery();
		    }
		}
	    }
	    catch(Exception ex)
	    {
	        var exMsg = ex.Message;
	 	Console.WriteLine($"An error occurred: {exMsg}");
	    }
	}
	public async Task SaveSongToFileSystem(IFormFile song)
	{
	    try
	    {
	        var filePath = Path.Combine(_tempDirectoryRoot, song.FileName);
		await SaveSongToFileSystemTemp(song, filePath);
		System.IO.File.Delete(filePath);


		DirectoryManager dirMgr = new DirectoryManager(_config, _song);
		dirMgr.CreateDirectory();
		filePath = dirMgr.SongDirectory;
		if (!song.FileName.EndsWith(".mp3"))
		    filePath += $"{song.FileName}.mp3";
		else
		    filePath += $"{song.FileName}";

		Console.WriteLine($"Full path {filePath}");

		using (var fileStream = new FileStream(filePath, FileMode.Create))
		{
		    await song.CopyToAsync(fileStream);
		    _song.SongPath = filePath;

		    Console.WriteLine($"Writing song to the directory: {filePath}");
		    Console.WriteLine("Song successfully saved");
		}
	    }
	    catch (Exception ex)
	    {
	        var exMsg = ex.Message;
		Console.WriteLine($"An error occurred: {exMsg}");
	    }
	}

        public async Task<List<Song>> RetrieveAllSongDetails()
        {
            try
            {
                InitializeResults();
                _songs = new List<Song>();
                _conn.Open();
                string query = "SELECT * FROM Songs";

                _cmd = new MySqlCommand(query, _conn);
                _cmd.ExecuteNonQuery();

                _dataDump = new MySqlDataAdapter(_cmd);
                _dataDump.Fill(_results);
                _dataDump.Dispose();

                await PopulateSongDetails();


                _conn.Close();

            }
            catch (Exception ex)
            {
                var exMsg = ex.Message;
                Console.WriteLine($"An error ocurred: {exMsg}");
            }

            return _songs;
        }
	public async Task<Song> RetrieveSongDetails(int id)
	{
	    DataTable results = new DataTable();

	    try
	    {
	        using (MySqlConnection conn = new MySqlConnection(_connectionString))
		{
		    conn.Open();
		    string query = "SELECT * FROM Songs WHERE Id=@Id";
		    using (MySqlCommand cmd = new MySqlCommand(query, conn))
		    {
		        cmd.Parameters.AddWithValue("@Id", id);
		        
			cmd.ExecuteNonQuery();

	                using (MySqlDataAdapter dataDump = new MySqlDataAdapter(cmd))
			{
			    dataDump.Fill(results);
			}
		    }
		}
	    }
	    catch (Exception ex)
	    {
	        var exMsg = ex.Message;
		Console.WriteLine($"An error occurred");
	    }	
	    
       	    DataRow row = results.Rows[0];

	    return new Song
	    {
	        Id = Int32.Parse(row["Id"].ToString()),
		Filename = row["Filename"].ToString(),
		SongPath = row["SongPath"].ToString()
	    };
	}
	public async Task<SongData> RetrieveSong(int id)
	{
	    SongData song = new SongData();
	    try
	    {
	        _song = await RetrieveSongDetails(id);
		song = await RetrieveSongFromFileSystem(_song);
	    }
	    catch (Exception ex)
	    {
	        var exMsg = ex.Message;
		Console.WriteLine($"An error occurred: {exMsg}");
	    }

	    return song;
	}
	public async Task<SongData> RetrieveSong(Song songMetaData)
	{
	    SongData song = new SongData();
	    try
	    {
		Console.WriteLine("Fetching song from filesystem");
		song = await RetrieveSongFromFileSystem(songMetaData);
	    }
	    catch (Exception ex)
	    {
	        var exMsg = ex.Message;
		Console.WriteLine($"An error occurred: {exMsg}");
	    }

	    return song;
	}


	Song RetrieveMetaData(string filePath)
	{
	    string title, artist, album, genre;
	    int year, duration;
			
	    TagLib.File tfile = TagLib.File.Create(filePath);
	    duration = (int)tfile.Properties.Duration.TotalSeconds;


	    using (var mp3 = new Mp3(filePath))
    	    {
                Id3Tag tag = mp3.GetTag(Id3TagFamily.Version2X);
		title = tag.Title;
		artist = tag.Artists;
	        album = tag.Album;
		genre = "Not Implemented";
		year = (int)tag.Year;
        	Console.WriteLine("Title: {0}", title);
        	Console.WriteLine("Artist: {0}", artist);
        	Console.WriteLine("Album: {0}", album);
		Console.WriteLine("Genre: {0}", genre);
		Console.WriteLine("Year: {0}", year);
		Console.WriteLine("Duration: {0}", duration);
    	    }

	    _song = new Song
	    {
	        Title = title,
		Artist = artist,
		Album = album,
		Year = year,
		Genre = genre,
		Duration = duration,
		SongPath = filePath
	    };

	    return _song;
	}

	async Task<SongData> RetrieveSongFromFileSystem(Song details)
	{
            byte[] uncompressedSong = System.IO.File.ReadAllBytes(details.SongPath);
			
	    return new SongData
	    {
	        Data = uncompressedSong
	    };
	}


	async Task SaveSongToFileSystemTemp(IFormFile song, string filePath)
	{
	    using (var fileStream = new FileStream(filePath, FileMode.Create))
	    {
	        await song.CopyToAsync(fileStream);
		_song = RetrieveMetaData(filePath);
		_song.Filename = song.FileName;
	    }
	}

	void Initialize()
	{
	    try
	    {
	        _connectionString = _config.GetConnectionString("IcarusDev");
	    }
	    catch (Exception ex)
	    {
	        Console.WriteLine($"Error Occurred: {ex.Message}");
	    }
			
	}
        void InitializeConnection()
        {
            _conn = new MySqlConnection(_connectionString);
        }
        void InitializeResults()
        {
            _results = new DataTable();
        }
        async Task PopulateSongDetails()
        {
            foreach (DataRow row in _results.Rows)
            {
                Song song = new Song();
                foreach (DataColumn col in _results.Columns)
                {
                    string colStr = col.ToString().ToUpper();
                    switch (colStr)
                    {
                        case "ID":
                            song.Id = Int32.Parse(row[col].ToString());
                            break;
                        case "TITLE":
                            song.Title = row[col].ToString();
                            break;
                        case "ALBUM":
                            song.Album = row[col].ToString();
                            break;
                        case "ARTIST":
                            song.Artist = row[col].ToString();
                            break;
                        case "YEAR":
                            song.Year = Int32.Parse(row[col].ToString());
                            break;
                        case "GENRE":
                            song.Genre = row[col].ToString();
                            break;
                        case "DURATION":
                            song.Duration = Int32.Parse(row[col].ToString());
                            break;
                        case "FILENAME":
                            song.Filename = row[col].ToString();
                            break;
                        case "SONGPATH":
                            song.SongPath = row[col].ToString();
                            break;
                    }
                }
                _songs.Add(song);
            }
        }
        #endregion	
    }
}
