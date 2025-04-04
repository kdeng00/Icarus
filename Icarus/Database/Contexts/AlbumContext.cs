using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class AlbumContext : DbContext
{
    public DbSet<Album>? Albums { get; set; }

    public AlbumContext(DbContextOptions<AlbumContext> options) : base(options) { }
    public AlbumContext(string connString) : base(new DbContextOptionsBuilder<AlbumContext>()
                        .UseMySQL(connString).Options)
    {
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Album>()
            .ToTable("Album");

        modelBuilder.Entity<Album>(entity =>
        {
            entity.HasKey(e => e.Id);

            entity.Property(e => e.Id)
                .HasColumnType("binary(16)");
        });
    }

    public Album RetrieveRecord(Album album)
    {
        var albm = Albums!.FirstOrDefault(alb => alb.Id == album.Id);
        return albm!;
    }

    public bool DoesRecordExist(Album album)
    {
        return Albums!.FirstOrDefault(alb => alb.Id == album.Id) != null ? true : false;
    }
}
