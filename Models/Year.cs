using System;
using System.Collections.Generic;

using Newtonsoft.Json;

namespace Icarus.Models
{
	public class Year
	{
		[JsonProperty("id")]
		public int YearId { get; set; }
		[JsonProperty("year")]
		public int YearValue { get; set; }
		[JsonProperty("song_count")]
		public int SongCount { get; set; }

		[JsonIgnore]
		public List<Song> Songs { get; set; }
	}
}
