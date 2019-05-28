using System;
using System.Collections.Generic;

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Database.Contexts
{
	public class GenreContext : DbContext
	{
		public DbSet<Genre> Genres { get; set; }

		public GenreContext(DbContextOptions<GenreContext> options)
			: base(options)
		{ }

		protected override void OnModelCreating(ModelBuilder modelBuilder)
		{
			modelBuilder.Entity<Genre>()
				.ToTable("Genre");
		}
	}
}
