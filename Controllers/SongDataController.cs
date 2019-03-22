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

namespace Icarus.Controllers
{
    [Route("api/Song/data")]
    [ApiController]
    public class SongDataController : ControllerBase
    {
		#region Fields
		private IConfiguration _config;
		private SongManager _songMgr;
		private string _songDir;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongDataController(IConfiguration config)
		{
			_config = config;
			_songDir = _config.GetValue<string>("FilePath");
			_songMgr = new SongManager(config, _songDir);
		}
		#endregion


        [HttpGet]
        public ActionResult<IEnumerable<SongData>> Get()
        {
			List<SongData> songs = new List<SongData>();


            return songs;
        }

        [HttpGet("{id}")]
        public async Task<IActionResult> Get(int id)
        {
			SongData song = new SongData();

			song = await _songMgr.RetrieveSong(id);

			return File(song.Data, "application/x-msdownload", _songMgr.SongDetails.Filename);
        }

        [HttpPost]
        public async Task Post([FromForm(Name = "file")] List<IFormFile> songData)
        {
			try
			{
				Console.WriteLine("Uploading song...");

			    var uploads = _songDir;
				Console.WriteLine($"Song Root Path {uploads}");
				foreach (var sng in songData)
				{
					byte[] data;
            		if (sng.Length > 0) {
						await _songMgr.SaveSongToFileSystem(sng);
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
        }
    }
}
