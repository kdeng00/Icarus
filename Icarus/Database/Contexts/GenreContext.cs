using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class GenreContext : DbContext
{
    #region Properties
    public DbSet<Genre>? Genres { get; set; }
    #endregion

    public GenreContext(DbContextOptions<GenreContext> options) : base(options) { }
    public GenreContext(string connString) : base(new DbContextOptionsBuilder<GenreContext>()
                        .UseMySQL(connString).Options)
    {
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Genre>()
            .ToTable("Genre");

        modelBuilder.Entity<Genre>(entity =>
        {
            entity.HasKey(e => e.Id);

            entity.Property(e => e.Id)
                .HasColumnType("binary(16)");
        });
    }


    public Genre RetrieveRecord(Genre genre)
    {
        var gnre = Genres!.FirstOrDefault(gnr => gnr.Id == genre.Id);
        return gnre!;
    }

    public bool DoesRecordExist(Genre genre)
    {
        return Genres!.FirstOrDefault(gnr => gnr.Id == genre.Id) != null ? true : false;
    }
}
