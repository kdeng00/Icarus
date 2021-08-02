using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.IdentityModel.Tokens.Jwt;

using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.AspNetCore.Authorization;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.HttpsPolicy;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Microsoft.EntityFrameworkCore;
using MySql.Data;
using MySql.Data.EntityFrameworkCore.Extensions;
using MySql.Data.MySqlClient;    
using NLog;
using NLog.Web;
using NLog.Web.AspNetCore;

using Icarus.Authorization;
using Icarus.Authorization.Handlers;
using Icarus.Database.Contexts;
using Icarus.Database.Repositories;

namespace Icarus
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddControllers();

            string domain = $"https://{Configuration["Auth0:Domain"]}/";

            services.AddAuthentication(options =>
            {
                options.DefaultAuthenticateScheme = JwtBearerDefaults.AuthenticationScheme;
                options.DefaultChallengeScheme = JwtBearerDefaults.AuthenticationScheme;
            }).AddJwtBearer(options =>
            {
                options.Authority = domain;
                options.Audience = Configuration["Auth0:ApiIdentifier"];
            });

            services.AddAuthorization(options =>
            {
                options.AddPolicy("download:songs", policy => 
                policy.Requirements
                .Add(new HasScopeRequirement("download:songs", domain)));

                options.AddPolicy("download:cover_art", policy =>
                    policy.Requirements
                    .Add(new HasScopeRequirement("download:cover_art", domain)));

                options.AddPolicy("upload:songs", policy => 
                    policy.Requirements
                    .Add(new HasScopeRequirement("upload:songs", domain)));

                options.AddPolicy("delete:songs", policy =>
                    policy.Requirements
                    .Add(new HasScopeRequirement("delete:songs", domain)));

                options.AddPolicy("read:song_details", policy => 
                    policy.Requirements
                    .Add(new HasScopeRequirement("read:song_details", domain)));

                options.AddPolicy("update:songs", policy =>
                    policy.Requirements
                    .Add(new HasScopeRequirement("update:songs", domain)));

                options.AddPolicy("read:artists", policy =>
                    policy.Requirements
                    .Add(new HasScopeRequirement("read:artists", domain)));

                options.AddPolicy("read:albums", policy =>
                    policy.Requirements
                    .Add(new HasScopeRequirement("read:albums", domain)));

                options.AddPolicy("read:genre", policy =>
                    policy.Requirements
                    .Add(new HasScopeRequirement("read:genre", domain)));

                options.AddPolicy("read:year", policy =>
                    policy.Requirements
                    .Add(new HasScopeRequirement("read:year", domain)));

                options.AddPolicy("stream:songs", policy =>
                    policy.Requirements
                    .Add(new HasScopeRequirement("stream:songs", domain)));
            });


            services.AddSingleton<IAuthorizationHandler, HasScopeHandler>();

            var connString = Configuration.GetConnectionString("DefaultConnection");

            services.Add(new ServiceDescriptor(typeof(SongRepository), 
                new SongRepository(connString)));  

            services.Add(new ServiceDescriptor(typeof(AlbumRepository),
                new AlbumRepository(connString)));

            services.Add(new ServiceDescriptor(typeof(ArtistRepository),
                new ArtistRepository(connString)));

            services.Add(new ServiceDescriptor(typeof(GenreRepository),
                new GenreRepository(connString)));

            services.Add(new ServiceDescriptor(typeof(YearRepository),
                new YearRepository(connString)));

            services.Add(new ServiceDescriptor(typeof(CoverArtRepository),
                new CoverArtRepository(connString)));

            services.Add(new ServiceDescriptor(typeof(UserRepository), 
                new UserRepository(connString)));

            services.AddDbContext<SongContext>(options => options.UseMySQL(connString));
            services.AddDbContext<AlbumContext>(options => options.UseMySQL(connString));
            services.AddDbContext<ArtistContext>(options => options.UseMySQL(connString));
            services.AddDbContext<UserContext>(options => options.UseMySQL(connString));
            services.AddDbContext<GenreContext>(options => options.UseMySQL(connString));
            services.AddDbContext<YearContext>(options => options.UseMySQL(connString));
            services.AddDbContext<CoverArtContext>(options => options.UseMySQL(connString));
        }

        // Called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env)
        {
            // NOTE: Dev-related configuration can be done when env.IsDevelopment() evaluated to true

            app.UseRouting();
            app.UseAuthorization();
            app.UseEndpoints(endpoints =>
            {
                endpoints.MapControllers();
            });
        }
    }
}
