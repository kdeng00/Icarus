using Microsoft.EntityFrameworkCore;

namespace Icarus.Database.Contexts;

public class AccessLevelContext : DbContext
{
    public DbSet<Icarus.Models.AccessLevel>? AccessLevels { get; set; }

    public AccessLevelContext(DbContextOptions<AccessLevelContext> options) : base(options) { }
    public AccessLevelContext(string connString) : base(new DbContextOptionsBuilder<AccessLevelContext>()
        .UseMySQL(connString).Options)
    {

    }

    #region Methods
    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Icarus.Models.AccessLevel>().ToTable("AccessLevel");

        // modelBuilder.Entity<Icarus.Models.AccessLevel>().Property(m => m.Level).IsRequired(true);
    }
    #endregion
}
