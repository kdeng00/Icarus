using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.EntityFrameworkCore;
using Microsoft.IdentityModel.Tokens;
using Microsoft.OpenApi.Models;

using Icarus.Database.Contexts;


var builder = WebApplication.CreateBuilder(args);

var MAX_REQUEST_BODY_SIZE = 51200000000;


builder.Services.AddEndpointsApiExplorer();
builder.Services.AddSwaggerGen(c =>
{
    c.ResolveConflictingActions(apiDescriptions => apiDescriptions.First());

    c.SwaggerDoc("v1", new OpenApiInfo { Title = "Icarus", Version = "v1" });
    c.AddSecurityDefinition("Bearer", new OpenApiSecurityScheme()
    {
        Name = "Authorization",
        Scheme = "Bearer",
        BearerFormat = "JWT",
        Type = SecuritySchemeType.ApiKey,
        In = ParameterLocation.Header,
        Description = "Bearer *Auth Token*",
    });
    c.AddSecurityRequirement(new OpenApiSecurityRequirement
    {
        {
            new OpenApiSecurityScheme
            {
                Reference = new OpenApiReference
                {
                    Type = ReferenceType.SecurityScheme,
                    Id = "Bearer"
                }
            },
            new string[] {}
        }
    });
});

builder.Services.AddControllers();
builder.WebHost.UseKestrel(option =>
{
    option.Limits.MaxRequestBodySize = MAX_REQUEST_BODY_SIZE;
});

var Configuration = builder.Configuration;

var connString = Configuration.GetConnectionString("DefaultConnection");

builder.Services.AddAuthentication(JwtBearerDefaults.AuthenticationScheme).AddJwtBearer(options =>
{
    options.RequireHttpsMetadata = false;
    options.SaveToken = true;
    var audience = Configuration["JWT:Audience"];
    var issuer = Configuration["JWT:Issuer"];
    options.TokenValidationParameters = new TokenValidationParameters()
    {
        ValidateIssuer = true,
        ValidateAudience = true,
        ValidateIssuerSigningKey = true,
        ValidateLifetime = true,
        ValidAudience = audience,
        ValidIssuer = issuer,
        IssuerSigningKey = new SymmetricSecurityKey(System.Text.Encoding.UTF8.GetBytes(Configuration["JWT:Secret"]!))
    };
});

builder.Services.AddDbContext<SongContext>(options => options.UseMySQL(connString!));
builder.Services.AddDbContext<AlbumContext>(options => options.UseMySQL(connString!));
builder.Services.AddDbContext<ArtistContext>(options => options.UseMySQL(connString!));
builder.Services.AddDbContext<UserContext>(options => options.UseMySQL(connString!));
builder.Services.AddDbContext<GenreContext>(options => options.UseMySQL(connString!));
builder.Services.AddDbContext<CoverArtContext>(options => options.UseMySQL(connString!));

builder.Services.AddControllers()
    .AddNewtonsoftJson();

builder.Services.AddEndpointsApiExplorer();


var app = builder.Build();

// Configure the HTTP request pipeline.
if (app.Environment.IsDevelopment())
{
    app.UseSwagger();
    app.UseSwaggerUI();
}

// NOTE: This should be enabled at some point
// app.UseHttpsRedirection();

app.UseRouting();
app.UseAuthentication();
app.UseAuthorization();
#pragma warning disable ASP0014
app.UseEndpoints(endpoints =>
{
    endpoints.MapControllers();
});
#pragma warning restore ASP0014


app.Run();
