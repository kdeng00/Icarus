using System;    
using System.Collections.Generic;    

using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;    

using Icarus.Models;
        
namespace Icarus.Models.Context    
{    
	public class UserContext : DbContext
	{
        	public DbSet<User> Users { get; set; }


		public UserContext(DbContextOptions<UserContext> options)
            		: base(options)
        	{ }

        	protected override void OnModelCreating(ModelBuilder modelBuilder)
        	{
			modelBuilder.Entity<User>()
				.ToTable("User");
            		modelBuilder.Entity<User>()
				.Property(u => u.LastLogin).IsRequired(false);
			modelBuilder.Entity<User>()
				.Property(u => u.DateCreated).HasDefaultValue(DateTime.Now);
        	}
    	}
}  
