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
    public class AlbumContext : DbContext
    {
        public DbSet<Album> Albums { get; set; }

        public AlbumContext(DbContextOptions<AlbumContext> options) : base(options) { }
        public AlbumContext(string connString) : base(new DbContextOptionsBuilder<AlbumContext>()
                            .UseMySQL(connString).Options)
        {
        }                        

        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<Album>()
                .ToTable("Album");
        }

        public bool DoesRecordExist(Album album)
        {
            return Albums.FirstOrDefault(alb => alb.AlbumId == album.AlbumId) != null ? true : false;
        }
    }
}
