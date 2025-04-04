using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class CoverArtContext : DbContext
{
    #region Properties
    public DbSet<CoverArt>? CoverArtImages { get; set; }
    #endregion

    public CoverArtContext(DbContextOptions<CoverArtContext> options) : base(options) { }
    public CoverArtContext(string connString) : base(new DbContextOptionsBuilder<CoverArtContext>()
                        .UseMySQL(connString).Options)
    {
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<CoverArt>()
            .ToTable("CoverArt");
        modelBuilder.Entity<CoverArt>(entity =>
        {
            entity.HasKey(e => e.Id);

            entity.Property(e => e.Id)
                .HasColumnType("binary(16)");
        });
    }

    public CoverArt RetrieveRecord(CoverArt cover)
    {
        return CoverArtImages!.FirstOrDefault(cov => cov.Id == cover.Id)!;
    }

    public bool DoesRecordExist(CoverArt cover)
    {
        return CoverArtImages!.FirstOrDefault(cov => cov.Id == cover.Id) != null ? true : false;
    }
}
