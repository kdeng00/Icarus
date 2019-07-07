using System;    
using System.Collections.Generic;    

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;    

using Icarus.Models;
        
namespace Icarus.Database.Contexts
{    
    public class SongContext : DbContext
    {
        public DbSet<Song> Songs { get; set; }


	public SongContext(DbContextOptions<SongContext> options) : base(options) { }

       	protected override void OnModelCreating(ModelBuilder modelBuilder)
       	{
	    modelBuilder.Entity<Song>()
	        .ToTable("Song");

	    modelBuilder.Entity<Song>()
	        .HasOne(s => s.Album)
		.WithMany(al => al.Songs)
		.HasForeignKey(s => s.AlbumId)
		.OnDelete(DeleteBehavior.SetNull);

	    modelBuilder.Entity<Song>()
	        .HasOne(sa => sa.SongArtist)
		.WithMany(ar => ar.Songs)
		.HasForeignKey(s => s.ArtistId)
		.OnDelete(DeleteBehavior.SetNull);

	    modelBuilder.Entity<Song>()
	        .HasOne(s => s.SongGenre)
		.WithMany(gnr => gnr.Songs)
		.HasForeignKey(s => s.GenreId)
		.OnDelete(DeleteBehavior.SetNull);

	    modelBuilder.Entity<Song>()
	        .HasOne(s => s.SongYear)
		.WithMany(yr => yr.Songs)
		.HasForeignKey(s => s.YearId)
		.OnDelete(DeleteBehavior.SetNull);

	    modelBuilder.Entity<Song>()
		 .HasOne(s => s.SongCoverArt)
		 .WithMany(ca => ca.Songs)
		 .HasForeignKey(s => s.CoverArtId)
		 .OnDelete(DeleteBehavior.SetNull);

	    modelBuilder.Entity<Song>()
	        .Property(s => s.Year)
		.IsRequired(false);
	    modelBuilder.Entity<Song>()
	        .Property(s => s.YearId)
		.IsRequired(false);
	    modelBuilder.Entity<Song>()
	        .Property(s => s.GenreId)
		.IsRequired(false);
	    modelBuilder.Entity<Song>()
	        .Property(s => s.ArtistId)
		.IsRequired(false);
	    modelBuilder.Entity<Song>()
	        .Property(s => s.AlbumId)
		.IsRequired(false);
	    modelBuilder.Entity<Song>()
		.Property(s => s.CoverArtId)
		.IsRequired(false);
	}
    }
}  
