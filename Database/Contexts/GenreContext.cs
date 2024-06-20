using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class GenreContext : DbContext
{
    public DbSet<Genre> Genres { get; set; }
    public GenreContext(DbContextOptions<GenreContext> options) : base(options) { }
    public GenreContext(string connString) : base(new DbContextOptionsBuilder<GenreContext>()
                        .UseMySQL(connString).Options)
    {
    }                        

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Genre>()
            .ToTable("Genre");
    }


    public Genre RetrieveRecord(Genre genre)
    {
        return Genres.FirstOrDefault(gnr => gnr.Id == genre.Id);
    }

    public bool DoesRecordExist(Genre genre)
    {
        return Genres.FirstOrDefault(gnr => gnr.Id == genre.Id) != null ? true : false;
    }
}
