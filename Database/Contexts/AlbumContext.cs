using System;
using System.Collections.Generic;
using System.Linq;

using Microsoft.EntityFrameworkCore;

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

        public Album RetrieveRecord(Album album)
        {
            return Albums.FirstOrDefault(alb => alb.AlbumID == album.AlbumID);
        }

        public bool DoesRecordExist(Album album)
        {
            return Albums.FirstOrDefault(alb => alb.AlbumID == album.AlbumID) != null ? true : false;
        }
    }
}
