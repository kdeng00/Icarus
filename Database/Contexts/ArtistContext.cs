using System;
using System.Collections.Generic;
using System.Linq;

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore;
using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Database.Contexts
{
    public class ArtistContext : DbContext
    {
        public DbSet<Artist> Artists { get; set; }

		public ArtistContext(DbContextOptions<ArtistContext> options) : base (options) { }
		public ArtistContext(string connString) : base(new DbContextOptionsBuilder<ArtistContext>()
							.UseMySQL(connString).Options)
		{
		}                        

		protected override void OnModelCreating(ModelBuilder modelBuilder)
		{
			modelBuilder.Entity<Artist>()
				.ToTable("Artist");
		}


        public bool DoesRecordExist(Artist artist)
        {
            return Artists.FirstOrDefault(arst => arst.ArtistId == artist.ArtistId) != null ? true : false;
        }
    }
}
