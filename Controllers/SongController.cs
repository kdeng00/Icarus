﻿using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Models;

namespace Icarus.Controllers
{
    [Route("api/Song")]
    [ApiController]
    public class SongController : ControllerBase
    {
		#region Fields
		private IConfiguration _config;
		private SongManager _songMgr;
		#endregion


		#region Properties
		#endregion


		#region Constructor
		public SongController(IConfiguration config)
		{
			_config = config;
			_songMgr = new SongManager(config);
		}
		#endregion


        [HttpGet]
        public ActionResult<IEnumerable<Song>> Get()
        {
			List<Song> songs = new List<Song>();
            songs = _songMgr.RetrieveAllSongDetails().Result;


            return songs;
        }

        [HttpGet("{id}")]
        public ActionResult<Song> Get(int id)
        {
			Song song = _songMgr.RetrieveSongDetails(id).Result;

            return song;
        }

        [HttpPost]
        public void Post([FromBody] Song song)
        {
			_songMgr.SaveSongDetails(song);
        }

        [HttpPut("{id}")]
        public void Put(int id, [FromBody] Song song)
        {
        }

        [HttpDelete("{id}")]
        public void Delete(int id)
        {
        }
    }
}