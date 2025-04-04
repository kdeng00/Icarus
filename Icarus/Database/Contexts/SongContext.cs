using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class SongContext : DbContext
{
    public DbSet<Song>? Songs { get; set; }

    public SongContext(string connString) : base(new DbContextOptionsBuilder<SongContext>()
                        .UseMySQL(connString).Options)
    {
    }


    public SongContext(DbContextOptions<SongContext> options) : base(options) { }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Song>()
            .ToTable("Song");

        modelBuilder.Entity<Song>(entity => 
        {
            entity.HasKey(e => e.Id);

            entity.Property(e => e.Id)
                .HasColumnType("binary(16)");
        });

        modelBuilder.Entity<Song>()
            .Property(s => s.Year)
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


    public Song RetrieveRecord(Song song)
    {
        var sng = Songs!.FirstOrDefault(sng => sng.Id == song.Id);
        return sng!;
    }


    public bool DoesRecordExist(Song song)
    {
        return Songs!.FirstOrDefault(sng => sng.Id == song.Id) != null ? true : false;
    }
}
