using System;    
using System.Collections.Generic;    

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;    

using Icarus.Models;
        
namespace Icarus.Models.Context    
{    
	public class SongContext : DbContext
    	{
        	public DbSet<Song> Songs { get; set; }


		public SongContext(DbContextOptions<SongContext> options)
            		: base(options)
        	{ }

        	protected override void OnModelCreating(ModelBuilder modelBuilder)
        	{
            		modelBuilder.Entity<Song>()
				.ToTable("Song")
				.HasOne(s => s.Album)
				.WithMany(a => a.Songs)
				.HasForeignKey(s => s.AlbumId)
				.HasConstraintName("ForeignKey_Song_Album");
        	}
    	}
}  
