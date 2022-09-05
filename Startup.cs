using System;
using System.Text;

using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Microsoft.Extensions.Options;
using Microsoft.EntityFrameworkCore;
using Microsoft.IdentityModel.Tokens;
using Newtonsoft.Json;
using NLog;
using NLog.Web;
using NLog.Web.AspNetCore;

using Icarus.Database.Contexts;

namespace Icarus
{
    public static class ServiceStartup
    {
        public static IServiceCollection AddAsymmetricAuthentication(this IServiceCollection services, IConfiguration configuration)
        {
            var issuerSigningCertificate = new Icarus.Certs.SigningIssuerCertificate();
            RsaSecurityKey issuerSigningKey = issuerSigningCertificate.GetIssuerSigningKey(configuration["RSAKeys:PublicKeyPath"]);

            services.AddAuthentication(authOptions =>
                {
                })
                .AddJwtBearer(options =>
                {
                    options.TokenValidationParameters = new TokenValidationParameters
                    {
                        IssuerSigningKey = issuerSigningKey,
                    };
                });

            return services;
        }

        private static bool LifetimeValidator(DateTime? notBefore,
            DateTime? expires,
            SecurityToken securityToken,
            TokenValidationParameters validationParameters)
        {
            return expires != null && expires > DateTime.UtcNow;
        }
    }
    public class Startup
    {
        #region Constructors
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }
        #endregion

        #region Properties
        public IConfiguration Configuration { get; }
        #endregion


        #region Methods
        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddControllers();

            var auth_id = Configuration["Auth0:Domain"];
            var domain = $"https://{auth_id}/";
            var audience = Configuration["Auth0:ApiIdentifier"];

            var connString = Configuration.GetConnectionString("DefaultConnection");

            services.AddAuthentication(JwtBearerDefaults.AuthenticationScheme).AddJwtBearer(options =>
            {
                options.RequireHttpsMetadata = false;
                options.SaveToken = true;
                options.TokenValidationParameters = new TokenValidationParameters()
                {
                    ValidateIssuer = true,
                    ValidateAudience = true,
                    ValidAudience = Configuration["JWT:Audience"],
                    ValidIssuer = Configuration["JWT:Issuer"],
                    IssuerSigningKey = new SymmetricSecurityKey(Encoding.UTF8.GetBytes(Configuration["JWT:Secret"]))
                };
            });

            services.AddDbContext<SongContext>(options => options.UseMySQL(connString));
            services.AddDbContext<AlbumContext>(options => options.UseMySQL(connString));
            services.AddDbContext<ArtistContext>(options => options.UseMySQL(connString));
            services.AddDbContext<UserContext>(options => options.UseMySQL(connString));
            services.AddDbContext<GenreContext>(options => options.UseMySQL(connString));
            services.AddDbContext<CoverArtContext>(options => options.UseMySQL(connString));

            // services.AddAsymmetricAuthentication(Configuration);

            /**
            services.AddTransient<AuthenticationService>(au => new AuthenticationService(new UserService(Configuration), new TokenService(Configuration), Configuration));
            services.AddTransient<UserService>(us => new UserService(Configuration));
            services.AddTransient<TokenService>(tk => new TokenService(Configuration));
            services.AddTransient<UserRepository>();
            services.AddTransient<UserContext>(uc => new UserContext(connString));
            */

            services.AddControllers()
                .AddNewtonsoftJson();
        }

        // Called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            // NOTE: Dev-related configuration can be done when env.IsDevelopment() evaluated to true

            app.UseRouting();
            app.UseAuthentication();
            app.UseAuthorization();
            app.UseEndpoints(endpoints =>
            {
                endpoints.MapControllers();
            });
        }
        #endregion
    }
}
