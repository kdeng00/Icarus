using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Security.Cryptography;
using System.Security.Cryptography.X509Certificates;

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
using Microsoft.IdentityModel.Tokens;
using Newtonsoft.Json;
using NLog;
using NLog.Web;
using NLog.Web.AspNetCore;
using Org.BouncyCastle.Crypto;
using Org.BouncyCastle.Crypto.Parameters;
using Org.BouncyCastle.OpenSsl;
using Org.BouncyCastle.Security;

using Icarus.Services;
using Icarus.Repositories;
using Icarus.Authorization;
using Icarus.Authorization.Handlers;
using Icarus.Database.Contexts;

namespace Icarus
{
    #region Classes
    public class SigningIssuerCertificate
    {
        public SigningIssuerCertificate()
        {
        }

        // public or private key?
        public RsaSecurityKey GetIssuerSigningKey(string publicKeyPath)
        {
            var file = publicKeyPath;
            var publicKey = System.IO.File.ReadAllText(file);
            var rsa = new RSACryptoServiceProvider();

            using (var reader = System.IO.File.OpenText(file))
            {
                var pem = new PemReader(reader);
                var o = (RsaKeyParameters)pem.ReadObject();
                var parameters = new RSAParameters();
                parameters.Modulus = o.Modulus.ToByteArray();
                parameters.Exponent = o.Exponent.ToByteArray();
                rsa.ImportParameters(parameters);
            }

            return new RsaSecurityKey(rsa);
        }
    }

    public class SigningAudienceCertificate
    {
        // public or private key?
        public SigningCredentials GetAudienceSigningKey(string keyPath)
        {
            // string privateXmlKey = System.IO.File.ReadAllText("./private_key.xml");
            // rsa.FromXmlString(privateXmlKey);

            var file = keyPath;
            var publicKey = System.IO.File.ReadAllText(file);
            var rsa = new RSACryptoServiceProvider();

            using (var reader = System.IO.File.OpenText(file))
            {
                var pem = new PemReader(reader);
                var o = (RsaKeyParameters)pem.ReadObject();
                var parameters = new RSAParameters();
                parameters.Modulus = o.Modulus.ToByteArray();
                parameters.Exponent = o.Exponent.ToByteArray();
                rsa.ImportParameters(parameters);
            }

            return new SigningCredentials(
                key: new RsaSecurityKey(rsa),
                algorithm: SecurityAlgorithms.RsaSha256);
        }
    }
    #endregion

    public static class MyClass
    {
        public static IServiceCollection AddAsymmetricAuthentication(this IServiceCollection services, IConfiguration configuration)
        // public static IServiceCollection AddAsymmetricAuthentication(this IServiceCollection services, IConfiguration configuration)
        {
            var issuerSigningCertificate = new SigningIssuerCertificate();
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
            /**

            services
                .AddAuthentication(JwtBearerDefaults.AuthenticationScheme)
                .AddJwtBearer(options => 
                {
                    options.Authority = domain;
                    options.Audience = audience;
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
            */

            var connString = Configuration.GetConnectionString("DefaultConnection");


            services.AddDbContext<SongContext>(options => options.UseMySQL(connString));
            services.AddDbContext<AlbumContext>(options => options.UseMySQL(connString));
            services.AddDbContext<ArtistContext>(options => options.UseMySQL(connString));
            services.AddDbContext<UserContext>(options => options.UseMySQL(connString));
            services.AddDbContext<GenreContext>(options => options.UseMySQL(connString));
            services.AddDbContext<CoverArtContext>(options => options.UseMySQL(connString));

            services.AddAsymmetricAuthentication(Configuration);

            services.AddTransient<AuthenticationService>(au => new AuthenticationService(new UserService(new UserRepository(), Configuration), new TokenService(new UserRepository(), Configuration), Configuration));
            services.AddTransient<UserService>(us => new UserService(new UserRepository(), Configuration));
            services.AddTransient<TokenService>(tk => new TokenService(new UserRepository(), Configuration));
            services.AddTransient<UserRepository>();
            services.AddTransient<UserContext>(uc => new UserContext(connString));
            /**
            services.AddTransient<Func<string, UserContext>((providers) =>
            {
                return new Func<string, UserContext>((numParams) =>
                    new UserContext(providers.GetRequiredService<IConfiguration>, numParams));
            });
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
