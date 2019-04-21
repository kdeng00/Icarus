using System;
using System.Collections.Generic;
using System.Configuration;
using System.Linq;
using System.Threading.Tasks;

using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;

using Icarus.Controllers.Managers;
using Icarus.Models;
using Icarus.Models.Context;

namespace Icarus.Controllers
{
    [Route("api/Song")]
    [ApiController]
    public class SongController : ControllerBase
    {
	#region Fields
	private IConfiguration _config;
	private MusicStoreContext _context;
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
            //songs = _songMgr.RetrieveAllSongDetails().Result;
	    Console.WriteLine("Attemtping to retrieve songs");

	    MusicStoreContext context = HttpContext.RequestServices
		    				   .GetService(typeof(MusicStoreContext)) 
						   as MusicStoreContext;

	    songs = context.GetAllSongs();


            return songs;
        }

        [HttpGet("{id}")]
        public ActionResult<Song> Get(int id)
        {
	    MusicStoreContext context = HttpContext.RequestServices
		    				   .GetService(typeof(MusicStoreContext)) 
						   as MusicStoreContext;
	    Song song = context.GetSong(id);

            return song;
        }

        [HttpPost]
        public void Post([FromBody] Song song)
        {
	    MusicStoreContext context = HttpContext.RequestServices
		    				   .GetService(typeof(MusicStoreContext)) 
						   as MusicStoreContext;

	    context.SaveSong(song);
        }

        [HttpPut("{id}")]
        public void Put(int id, [FromBody] Song song)
        {
        }

        [HttpDelete("{id}")]
        public void Delete(int id)
        {
	    MusicStoreContext context = HttpContext.RequestServices
		    				   .GetService(typeof(MusicStoreContext)) 
						   as MusicStoreContext;

	    context.DeleteSong(id);
        }
    }
}
