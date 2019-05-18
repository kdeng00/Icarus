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
				.ToTable("Song");

			modelBuilder.Entity<Song>()
				.HasOne(s => s.Album)
				.WithMany(al => al.Songs)
				.HasForeignKey(s => s.AlbumId)
				.HasConstraintName("ForeignKey_Song_Album");

			modelBuilder.Entity<Song>()
				.HasOne(sa => sa.SongArtist)
				.WithMany(ar => ar.Songs)
				.HasForeignKey(s => s.ArtistId)
				.HasConstraintName("ForeignKey_Song_Artist");

			modelBuilder.Entity<Song>()
				.HasOne(s => s.SongGenre)
				.WithMany(gnr => gnr.Songs)
				.HasForeignKey(s => s.GenreId)
				.HasConstraintName("HasForeignKey_Song_Genre");

			modelBuilder.Entity<Song>()
				.HasOne(s => s.SongYear)
				.WithMany(yr => yr.Songs)
				.HasForeignKey(s => s.YearId)
				.HasConstraintName("HasForeignKey_Song_Year");

			modelBuilder.Entity<Song>()
				.Property(s => s.Year)
				.IsRequired(false);
        	}
    	}
}  
