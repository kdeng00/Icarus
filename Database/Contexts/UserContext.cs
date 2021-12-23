using System;    
using System.Collections.Generic;    
using System.Linq;

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;    

using Icarus.Models;
        
namespace Icarus.Database.Contexts
{    
    public class UserContext : DbContext
    {
        public DbSet<User> Users { get; set; }


        public UserContext(DbContextOptions<UserContext> options) : base(options) { }
        public UserContext(string connString) : base(new DbContextOptionsBuilder<UserContext>()
                            .UseMySQL(connString).Options)
        {
        }                        

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<User>()
                .ToTable("User");
            modelBuilder.Entity<User>()
                .Property(u => u.LastLogin).IsRequired(false);
            modelBuilder.Entity<User>()
                .Property(u => u.DateCreated).HasDefaultValue(DateTime.Now);
        }


        public bool DoesRecordExist(User user)
        {
            return Users.FirstOrDefault(usr => usr.UserID == user.UserID) != null ? true : false;
        }
    }
}  
