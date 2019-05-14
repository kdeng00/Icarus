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
using Icarus.Models.Context;

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
			services.AddMvc().SetCompatibilityVersion(CompatibilityVersion.Version_2_2);
			services.AddSingleton<IConfiguration>(Configuration);

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
						policy
						.Requirements
						.Add(new HasScopeRequirement("download:songs", domain)));

				options.AddPolicy("upload:songs", policy => 
						policy
						.Requirements
						.Add(new HasScopeRequirement("upload:songs", domain)));

				options.AddPolicy("delete:songs", policy =>
						policy
						.Requirements
						.Add(new HasScopeRequirement("delete:songs", domain)));

				options.AddPolicy("read:song_details", policy => 
						policy
						.Requirements
						.Add(new HasScopeRequirement("read:song_details", domain)));

				options.AddPolicy("update:songs", policy =>
						policy
						.Requirements
						.Add(new HasScopeRequirement("update:songs", domain)));

				options.AddPolicy("read:artists", policy =>
						policy
						.Requirements
						.Add(new HasScopeRequirement("read:artists", domain)));

				options.AddPolicy("read:albums", policy =>
						policy
						.Requirements
						.Add(new HasScopeRequirement("read:albums", domain)));
			});


			services.AddSingleton<IAuthorizationHandler, HasScopeHandler>();

			var connString = Configuration.GetConnectionString("DefaultConnection");

			// TODO: Add the Genre and Year stores #41 and #42.
	    		services.Add(new ServiceDescriptor(typeof(MusicStoreContext), 
						new MusicStoreContext(Configuration
							.GetConnectionString("DefaultConnection"))));  

			services.Add(new ServiceDescriptor(typeof(AlbumStoreContext),
						new AlbumStoreContext(Configuration
							.GetConnectionString("DefaultConnection"))));

			services.Add(new ServiceDescriptor(typeof(ArtistStoreContext),
						new ArtistStoreContext(Configuration
							.GetConnectionString("DefaultConnection"))));

			services.Add(new ServiceDescriptor(typeof(UserStoreContext), 
						new UserStoreContext(Configuration
							.GetConnectionString("DefaultConnection"))));

			services.AddDbContext<SongContext>(options => options.UseMySQL(connString));
			services.AddDbContext<AlbumContext>(options => options.UseMySQL(connString));
			services.AddDbContext<ArtistContext>(options => options.UseMySQL(connString));
			services.AddDbContext<UserContext>(options => options.UseMySQL(connString));
			// TODO: Add the Genre and Year contexts #41 and #42
        	}

        	// This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        	public void Configure(IApplicationBuilder app, IHostingEnvironment env)
        	{
            		if (env.IsDevelopment())
            		{
                		app.UseDeveloperExceptionPage();
            		}
            		else
            		{
                		// The default HSTS value is 30 days. You may want to change this for production scenarios, see https://aka.ms/aspnetcore-hsts.
               			 app.UseHsts();
            		}

			app.UseAuthentication();

            		app.UseHttpsRedirection();
            		app.UseMvc();
        	}
    	}
}
