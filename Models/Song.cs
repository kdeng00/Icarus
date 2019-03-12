using System;

namespace Icarus.Models
{
	public class Song
	{
		public string Title { get; set; }
		public string Album { get; set; }
		public string Artist { get; set; }
		public int Year { get; set; }
		public string Genre { get; set; }
		public int Duration { get; set; }
		public byte[] SongFile { get; set; }
	}
}
