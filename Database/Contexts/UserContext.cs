using System;    
using System.Collections.Generic;    
using System.Linq;

// using MySql.Data.EntityFrameworkCore;
// using MySql.Data;
// using MySql.Data.EntityFrameworkCore.Extensions;
// using MySql.Data.Entity;
// using MySql.Data.MySqlClient;    
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;

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
        modelBuilder.Entity<User>()
            .Property(u => u.LastLogin).IsRequired(false);
        modelBuilder.Entity<User>()
            .Property(u => u.DateCreated).HasDefaultValue(DateTime.Now);
    }

    
    public User RetrieveRecord(User user)
    {
        return Users.FirstOrDefault(usr => usr.UserID == user.UserID);
    }

    public bool DoesRecordExist(User user)
    {
        return Users.FirstOrDefault(usr => usr.UserID == user.UserID) != null ? true : false;
    }
}
