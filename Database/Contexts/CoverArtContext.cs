using System;
using System.Collections.Generic;

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;

using Icarus.Models;

namespace Icarus.Database.Contexts
{
    public class CoverArtContext : DbContext
    {
        public DbSet<CoverArt> CoverArtImages { get; set; }

	public CoverArtContext(DbContextOptions<CoverArtContext> options) : base(options) { }
	
	protected override void OnModelCreating(ModelBuilder modelBuilder)
	{
	    modelBuilder.Entity<CoverArt>()
	        .ToTable("CoverArt");
	}
    }
}
