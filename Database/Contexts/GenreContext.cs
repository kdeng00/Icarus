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


        public bool DoesRecordExist(Genre genre)
        {
            return Genres.FirstOrDefault(gnr => gnr.GenreId == genre.GenreId) != null ? true : false;
        }
    }
}
