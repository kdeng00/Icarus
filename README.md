# Icarus

Icarus is a music streaming API Server that interacts with [Mear](https://github.com/amazing-username/mear).

### Interfacing With Icarus

One can interface with Icarus the music server either by:

* [Mear](https://github.com/amazing-username/mear) - Feature not implemented (under development)
* [IcarusDownloadManager](https://github.com/amazing-username/IcarusDownloadManager) - Partially Implemented (under development)



## Built With


* C#
* [.NET Core](https://dotnet.microsoft.com/) 2.2
* .NET Web RESTful API
* [MySql](https://www.nuget.org/packages/MySql.Data/)
* [Newtonsoft.Json](https://www.newtonsoft.com/json)
* [TagLib#](https://github.com/mono/taglib-sharp)

![image](https://user-images.githubusercontent.com/14333136/56252069-28532d00-6084-11e9-896d-1a3c378014ef.png)

## Getting started
There are several things that need to be completed to properly setup and secure the API.
1. Auth0 API configuration
2. API filesystem paths
3. Database connection string
4. Migrations

### Auth0 API configuration

### API filesystem paths

### Database connection string

In order for Database functionality to be operable, there must be a valid connection string and credentials with appropriate permissions. At the moment there is only support for MySQL. Depending on your environment `Release` or `Debug` you will need to edit the appsettings.json or appsettings.Development.json accordingly. An example of the fields to change are below:
```Json
{

  "ConnectionStrings": {
	  "DefaultConnection": "Server=;Database=;Uid=;Pwd=;"
  }
 
}
```
* Server - The address or domain name of the MySQL server
* Database - The database name
* Uid - Username
* Password - Self-explanatory

The only requirement of the User is that the user should have full permissions to the database as well as permissions to create a database. Other than that, that is all that is required.

### Migrations

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on the code of conduct, and the process for submitting pull requests to the project.

## Versioning

Currently under development. No version has been released

## Authors

* **Kun Deng** - [amazing-username](https://github.com/amazing-username)

See also the list of [contributors](https://github.com/amazing-username/Icarus/graphs/contributors) who participated in this project.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

