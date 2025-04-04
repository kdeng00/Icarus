using Microsoft.EntityFrameworkCore;

using Icarus.Models;

namespace Icarus.Database.Contexts;

public class UserContext : DbContext
{
    public DbSet<User> Users { get; set; }


    #region Constructors
    public UserContext(DbContextOptions<UserContext> options) : base(options) { }
    [ActivatorUtilitiesConstructor]
    public UserContext(string connString) : base(new DbContextOptionsBuilder<UserContext>()
                        .UseMySQL(connString).Options)
    {
    }
    #endregion


    protected override void OnModelCreating(ModelBuilder modelBuilder)
    {
        modelBuilder.Entity<User>()
            .ToTable("User");

        modelBuilder.Entity<User>(entity =>
        {
            entity.HasKey(e => e.Id);

            entity.Property(e => e.Id)
                .HasColumnType("binary(16)"); // **** Map Guid to BINARY(16) ****
        });
        modelBuilder.Entity<User>()
            .Property(u => u.LastLogin).IsRequired(false);
        modelBuilder.Entity<User>()
            .Property(u => u.DateCreated).HasDefaultValue(DateTime.Now);
    }


    public User RetrieveRecord(User user)
    {
        return Users.FirstOrDefault(usr => usr.Id == user.Id)!;
    }

    public bool DoesRecordExist(User user)
    {
        return Users.FirstOrDefault(usr => usr.Id == user.Id) != null ? true : false;
    }
}
