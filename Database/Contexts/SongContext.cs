using System;    
using System.Collections.Generic;    
using System.Linq;

using Microsoft.EntityFrameworkCore;

using Icarus.Models;
        
namespace Icarus.Database.Contexts;

public class SongContext : DbContext
{
    public DbSet<Song> Songs { get; set; }

    public SongContext(string connString) : base(new DbContextOptionsBuilder<SongContext>()
                        .UseMySQL(connString).Options)
    {
    }                        


    public SongContext(DbContextOptions<SongContext> options) : base(options) { }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Song>()
            .ToTable("Song");

        /**
        modelBuilder.Entity<Song>()
            .HasOne(s => s.Album)
        .WithMany(al => al.Songs)
        .HasForeignKey(s => s.AlbumID)
        .OnDelete(DeleteBehavior.SetNull);

        modelBuilder.Entity<Song>()
            .HasOne(sa => sa.SongArtist)
        .WithMany(ar => ar.Songs)
        .HasForeignKey(s => s.ArtistID)
        .OnDelete(DeleteBehavior.SetNull);

        modelBuilder.Entity<Song>()
            .HasOne(s => s.SongGenre)
        .WithMany(gnr => gnr.Songs)
        .HasForeignKey(s => s.GenreID)
        .OnDelete(DeleteBehavior.SetNull);

        modelBuilder.Entity<Song>()
            .HasOne(s => s.SongYear)
        .WithMany(yr => yr.Songs)
        .HasForeignKey(s => s.YearID)
        .OnDelete(DeleteBehavior.SetNull);

        modelBuilder.Entity<Song>()
            .HasOne(s => s.SongCoverArt)
            .WithMany(ca => ca.Songs)
            .HasForeignKey(s => s.CoverArtID)
            .OnDelete(DeleteBehavior.SetNull);
        */

        modelBuilder.Entity<Song>()
            .Property(s => s.Year)
        .IsRequired(false);
        modelBuilder.Entity<Song>()
            .Property(s => s.GenreID)
        .IsRequired(false);
        modelBuilder.Entity<Song>()
            .Property(s => s.ArtistID)
        .IsRequired(false);
        modelBuilder.Entity<Song>()
            .Property(s => s.AlbumID)
        .IsRequired(false);
        modelBuilder.Entity<Song>()
        .Property(s => s.CoverArtID)
        .IsRequired(false);
    }


    public Song RetrieveRecord(Song song)
    {
        return Songs.FirstOrDefault(sng => sng.SongID == song.SongID);
    }


    public bool DoesRecordExist(Song song)
    {
        return Songs.FirstOrDefault(sng => sng.SongID == song.SongID) != null ? true : false;
    }
}
