using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class ArtistContext : DbContext
{
    public DbSet<Artist> Artists { get; set; }

    public ArtistContext(DbContextOptions<ArtistContext> options) : base (options) { }
    public ArtistContext(string connString) : base(new DbContextOptionsBuilder<ArtistContext>()
                        .UseMySQL(connString).Options)
    {
    }                        

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Artist>()
            .ToTable("Artist");
    }

    public Artist RetrieveRecord(Artist artist)
    {

        return Artists.FirstOrDefault(arst => arst.Id == artist.Id);
    }


    public bool DoesRecordExist(Artist artist)
    {
        return Artists.FirstOrDefault(arst => arst.Id == artist.Id) != null ? true : false;
    }
}
