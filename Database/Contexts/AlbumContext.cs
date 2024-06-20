using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class AlbumContext : DbContext
{
    public DbSet<Album> Albums { get; set; }

    public AlbumContext(DbContextOptions<AlbumContext> options) : base(options) { }
    public AlbumContext(string connString) : base(new DbContextOptionsBuilder<AlbumContext>()
                        .UseMySQL(connString).Options)
    {
    }                        

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Album>()
            .ToTable("Album");
    }

    public Album RetrieveRecord(Album album)
    {
        return Albums.FirstOrDefault(alb => alb.Id == album.Id);
    }

    public bool DoesRecordExist(Album album)
    {
        return Albums.FirstOrDefault(alb => alb.Id == album.Id) != null ? true : false;
    }
}
