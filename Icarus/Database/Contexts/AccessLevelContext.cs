using Microsoft.EntityFrameworkCore;

namespace Icarus.Database.Contexts;

public class AccessLevelContext : DbContext
{
    #region Properties
    public DbSet<Models.AccessLevel>? AccessLevels { get; set; }
    #endregion

    #region Constructors
    public AccessLevelContext(DbContextOptions<AccessLevelContext> options) : base(options) { }
    public AccessLevelContext(string connString) : base(new DbContextOptionsBuilder<AccessLevelContext>()
        .UseMySQL(connString).Options)
    {
        if (this.AccessLevels == null)
        {
        }
    }
    #endregion

    #region Methods
    public Models.AccessLevel? GetAccessLevel(int songId)
    {
        var accessLevel = this.AccessLevels!.FirstOrDefault(acc => acc.SongId == songId);
        return accessLevel;
    }

    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<Models.AccessLevel>().ToTable("AccessLevel");

        modelBuilder.Entity<Models.AccessLevel>().Property(m => m.Level).IsRequired(true);
    }
    #endregion
}
