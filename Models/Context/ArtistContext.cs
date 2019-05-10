using System;
using System.Collections.Generic;

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore;
using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Models.Context
{
	public class ArtistContext : DbContext
	{
		public DbSet<Artist> Artists { get; set; }

		public ArtistContext(DbContextOptions<ArtistContext> options)
			: base (options)
		{ }

		protected override void OnModelCreating(ModelBuilder modelBuilder)
		{
			modelBuilder.Entity<Artist>()
				.ToTable("Artist");
		}
	}
}
