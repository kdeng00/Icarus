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
		private string _songDir = @"";
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongDataController(IConfiguration config)
		{
			_config = config;
			_songMgr = new SongManager(config);
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

			return File(song.Data, "application/x-msdownload", "dfdf.mp3");
        }

        [HttpPost]
        public async Task Post([FromForm(Name = "file")] List<IFormFile> songData)
        {
			try
			{
				Console.WriteLine("Uploading song...");

			    var uploads = Path.Combine(_songDir, "uploads");
				Console.WriteLine($"Song Path {uploads}");
				foreach (var sng in songData)
				{
					byte[] data;
            		if (sng.Length > 0) {

						using (var ms = new MemoryStream())
						{
      						sng.CopyTo(ms);
      						data = ms.ToArray();
    					}
						SongData songD = new SongData
						{
							Data = data
						};

						_songMgr.SaveSong(songD);


						/**
               			var filePath = Path.Combine(uploads, sng.FileName);
                		using (var fileStream = new FileStream(filePath, FileMode.Create)) {
                   			await sng.CopyToAsync(fileStream);
                		}
						*/
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
