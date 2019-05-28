using System;
using System.Collections.Generic;

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Database.Contexts
{
	public class YearContext : DbContext
	{
		public DbSet<Year> YearValues { get; set; }

		public YearContext(DbContextOptions<YearContext> options)
			: base(options)
		{ }

		protected override void OnModelCreating(ModelBuilder modelBuilder)
		{
			modelBuilder.Entity<Year>()
				.ToTable("Year");
		}
	}
}
