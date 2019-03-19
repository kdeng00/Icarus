using System;
using System.Text;

namespace Icarus.Models
{
	public class SongData
	{
		public int Id { get; set; }
		public byte[] Data { get; set; }
		public int SongId { get; set; }
	}
}
