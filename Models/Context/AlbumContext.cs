using System;
using System.Collections.Generic;

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Models.Context
{
	public class AlbumContext : DbContext
	{
		public DbSet<Album> Albums { get; set; }

		public AlbumContext(DbContextOptions<AlbumContext> options)
			: base(options)
		{ }

		protected override void OnModelCreating(ModelBuilder modelBuilder)
		{
			modelBuilder.Entity<Album>();
		}
	}
}
