using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;


public class AccessLevelContext : DbContext {
    public DbSet<AccessLevel> AccessLevels { get; set; }

    public AccessLevelContext(string connString) : base(new DbContextOptionsBuilder<AccessLevelContext>()
        .UseMySQL(connString).Options)
        {

        }

    #region Methods
    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
    }
    #endregion
}