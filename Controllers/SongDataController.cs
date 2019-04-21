using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
    [Route("api/song/data")]
    [ApiController]
    public class SongDataController : ControllerBase
    {
        #region Fields
        private IConfiguration _config;
	private SongManager _songMgr;
	private string _songTempDir;
	#endregion


	#region Properties
	#endregion


	#region Constructor
	public SongDataController(IConfiguration config)
	{
	    _config = config;
       	    _songTempDir = _config.GetValue<string>("TemporaryMusicPath");
	    _songMgr = new SongManager(config, _songTempDir);
	}
	#endregion


        [HttpGet("{id}")]
        public async Task<IActionResult> Get(int id)
        {
	    MusicStoreContext context = HttpContext.RequestServices
		    				   .GetService(typeof(MusicStoreContext)) 
						   as MusicStoreContext;
	    var songMetaData = context.GetSong(id); 

	    SongData song = await _songMgr.RetrieveSong(songMetaData);

	    return File(song.Data, "application/x-msdownload", songMetaData.Filename);
        }

        [HttpPost]
        public async Task Post([FromForm(Name = "file")] List<IFormFile> songData)
        {
	    try
	    {
	        MusicStoreContext context = HttpContext.RequestServices
						       .GetService(typeof(MusicStoreContext)) 
						       as MusicStoreContext;

	        Console.WriteLine("Uploading song...");

	        var uploads = _songTempDir;
		Console.WriteLine($"Song Root Path {uploads}");
		foreach (var sng in songData)
		{
            	    if (sng.Length > 0) {
		        await _songMgr.SaveSongToFileSystem(sng);
			var song = _songMgr.SongDetails;
			context.SaveSong(song);
			Console.WriteLine("Song successfully saved");
            	    }
		}
	    }
	    catch (Exception ex)
	    {
	        Console.WriteLine($"An error occurred: {ex.Message}");
	    }
        }

        [HttpPut("{id}")]
        public void Put(int id, [FromBody] SongData song)
        {
        }

        [HttpDelete("{id}")]
        public void Delete(int id)
        {
	    MusicStoreContext context = HttpContext.RequestServices
		     				   .GetService(typeof(MusicStoreContext)) 
						   as MusicStoreContext;

	    var songMetaData = context.GetSong(id);

	    var result = _songMgr.DeleteSongFromFileSystem(songMetaData);

	    if (result)
	    {
	        context.DeleteSong(songMetaData.Id);
	    }
        }
    }
}
