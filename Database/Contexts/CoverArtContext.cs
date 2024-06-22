using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class CoverArtContext : DbContext
{
    public DbSet<CoverArt> CoverArtImages { get; set; }

    public CoverArtContext(DbContextOptions<CoverArtContext> options) : base(options) { }
    public CoverArtContext(string connString) : base(new DbContextOptionsBuilder<CoverArtContext>()
                        .UseMySQL(connString).Options)
    {
    }                        
    
    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<CoverArt>()
            .ToTable("CoverArt");
    }

    public CoverArt RetrieveRecord(CoverArt cover)
    {
        return CoverArtImages.FirstOrDefault(cov => cov.Id == cover.Id);
    }

    public bool DoesRecordExist(CoverArt cover)
    {
        return CoverArtImages.FirstOrDefault(cov => cov.Id == cover.Id) != null ? true : false;
    }
}
