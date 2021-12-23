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
    public class CoverArtContext : DbContext
    {
        public DbSet<CoverArt> CoverArtImages { get; set; }

        public CoverArtContext(DbContextOptions<CoverArtContext> options) : base(options) { }
        public CoverArtContext(string connString) : base(new DbContextOptionsBuilder<CoverArtContext>()
                            .UseMySQL(connString).Options)
        {
        }                        
        
        protected override void OnModelCreating(ModelBuilder modelBuilder)
        {
            modelBuilder.Entity<CoverArt>()
                .ToTable("CoverArt");
        }


        public bool DoesRecordExist(CoverArt cover)
        {
            return CoverArtImages.FirstOrDefault(cov => cov.CoverArtId == cover.CoverArtId) != null ? true : false;
        }
    }
}
