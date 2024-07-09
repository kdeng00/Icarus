# Icarus

Icarus is a music streaming API Server that interacts with [Mear](https://github.com/kdeng00/mear).

### Interfacing With Icarus

One can interface with Icarus the music server either by:

* [Mear](https://github.com/kdeng00/mear) - Partially implemented (under development)
* [IcarusDownloadManager](https://github.com/kdeng00/IcarusDownloadManager)


## Built With

* C# [.NET](https://dotnet.microsoft.com/) 8
* [MySql](https://www.nuget.org/packages/MySql.Data/)
* OpenSSL
* BCrypt.Net-Next
* DotNetZip
* ID3
* JWT
* Microsoft.AspNetCore.Authentication.JwtBearer
* Microsoft.AspNetCore.Mvc.NewtonsoftJson
* Microsoft.EntityFrameworkCore
* Microsoft.EntityFrameworkCore.Tools
* MySql.EntityFrameworkCore
* [Newtonsoft.Json](https://www.newtonsoft.com/json)
* NLog.Web.AspNetCpre
* Portable.BouncyCastle
* RestSharp
* SevenZip
* System.IdentityModel.Tokens.Jwt
* [TagLib#](https://github.com/mono/taglib-sharp)



## Getting started

There are several things that need to be completed to properly setup and secure the API.
This API uses OpenAPI Specification 3.0. After configuring the API, launch the software
and navigate your browser to https://localhost:5001/swagger to view the endpoints.

1. JWT Information
2. API filesystem paths
3. Database connection string
4. Migrations


### JWT Information

Configure JWT information. Notably the Secret

```Json
  "JWT": {
    "Issuer": "IcarusAPI",
    "Audience": "IcarusAPIClient",
    "Secret": "Manaiswhatyouthinkitis",
    "Subject": "Authorization"
  },
```


Replace [domain] with the domain name that represent's your domain. Replace [identifier] with the identifer root name in the appsettings environment file. Not the friendly name but the root name of the identifier, omitting the http protocol and the *api* path.

```Json
"Auth0": {
  "Domain": "[domain].auth0.com", 
  "ApiIdentifier": "https://[identifier]/api"
},
```

**Note**: The Auth0 section is likely to be changed or removed in future releases.


### API filesystem paths

For the purposes of properly uploading, downloading, updating, deleting, and streaming songs the API filesystem paths must be configured. What is meant by this is that the `RootMusicPath` directory where all music will be stored must exist as well as the `ArchivePath` and `TemporaryMusicPath` paths. An example on a Linux system:
```Json
{
  "RootMusicPath": "/home/dev/null/music/",
  "TemporaryMusicPath": "/home/dev/null/music/temp/",
  "ArchivePath": "/home/dev/null/music/archive/",
  "CoverArtPath": "/home/dev/null/music/coverart/"
}
```
* RootMusicPath - Where music will be stored in the following convention: *`Artist/Album/Songs`*
* TemporaryMusicPath - Where music will be stored when uploding songs to the server until the metadata has been fully parsed and entered into the database. Upon completion the files will be deleted and moved to the appropriate path in the `RootMusicPath`
* ArchivePath - When downloading compressed songs this is the path where songs will be compressed prior to dataa being read into memory, deleting the compressed file, and sending the compressed file from memory to the client
* CoverArtPath - Root directory where cover art will be saved to


**Note**: The `TemporaryMusic` or `ArchivePath` does not have to be located in the `RootMusicPath`. Ensure that the permissions are properly set for all of the paths.

### Database connection string

In order for Database functionality to be operable, there must be a valid connection string and credentials with appropriate permissions. **At the moment there is only support for MySQL**. Depending on your environment `Release` or `Debug` you will need to edit the appsettings.json or appsettings.Development.json accordingly. An example of the fields to change are below:

```Json
{
  "ConnectionStrings": {
    "DefaultConnection": "Server=localhost;Database=my_db;Uid=admin;Pwd=toughpassword;"
  }
}
```

* Server - The address or domain name of the MySQL server
* Database - The database name
* Uid - Username
* Password - Self-explanatory

The only requirement of the User is that the user should have full permissions to the database as well as permissions to create a database. Other than that, that is all that is required.

### Migrations

Prior to starting the API, the Migrations must be applied. There are 6 tables with migrations being applied and thy are:
* Users
* Song
* Album
* Artist
* CoverArt
* Genre

There is a script for Linux systems to apply these migrations, it can be found in the [Scripts/Migrations/Linux](https://github.com/kdeng00/Icarus/blob/master/Scripts/Migrations/Linux/AddUpdate.sh) directory. Just merely execute:
```shell
scripts/Migrations/Linux/AddUpdate.sh
```
Or you can manually add the migrations like so for each migration:
```shell
dotnet dotnet-ef migrations Add InitialCreate --context UserContext
```
Then update the migrations to the database like so<sup>*</sup>:
```shell
dotnet dotnet-ef database update --context UserContext
```

All of the contexts can be found in Database/Contexts folder.

From this point the database has been successfully configured. Metadata and song filesystem locations can be saved.

<sup>*</sup> Will only need to execute this for UserContext and SongContext because the Song table has relational constraints with Album, Artist, Year, and Genre.

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on the code of conduct, and the process for submitting pull requests to the project.


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

