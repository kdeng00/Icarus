# Icarus

Icarus is a music streaming API Server, allowing access to stream your personal music collection

### Interfacing With Icarus

One can interface with Icarus the music server either by:

* [Mear](https://github.com/amazing-username/mear) - Partially implemented (under development)
* [IcarusDownloadManager](https://github.com/amazing-username/IcarusDownloadManager)
* Create your own client to interact with the API



## Built With


* C++ >= C++17
* GCC >= 8.0
* [json](https://www.github.com/nlohmann/json)
* [cpr](https://www.github.com/whoshuu/cpr)
* [TagLib](https://github.com/taglib/taglib)
* [jwt-cpp](https://github.com/Thalhammer/jwt-cpp)
* [libbcrypt](https://github.com/rg3/libbcrypt)
* [oatpp](https://github.com/oatpp/oatpp)

![image](https://user-images.githubusercontent.com/14333136/56252069-28532d00-6084-11e9-896d-1a3c378014ef.png)

## Getting started
There are several things that need to be completed to properly setup and secure the API.
1. Auth0 API configuration
2. API filesystem paths
3. Database connection string
4. Migrations

### Auth0 API configuration

Securing Icarus is required, preventing the API from being publicly accessible. To do so, create an Auth0 account (it's free), for the sake of this section of the documentation, I will not go over how to create an Auth0 account. Once created, create a tentant and proceed to create an API
<h1 align=center>
    <img src="Images/Configuration/create_api.png" width=100%>
</h1>

Create the API and enter an approrpiate name and identified. For the identified, append **api** like in the example
<h1 align="center">
    <img src="Images/Configuration/enter_api_info.png" width=100%>
</h1>
Replace [domain] with the domain name of the created tenant. This can be found in the Default App from the Application menu. Replace [identifier] with the identifer root name in the appsettings environment file. Not the friendly name but the root name of the identifier, omitting the http protocol and the *api* path.

```Json
  "domain": "[domain].auth0.com", 
  "api_identifier": "https://[identifier]/api",
  "client_id": "iamunique",
  "client_secret": "Icankeepasecret"
```

For the sake of this section, I will not go over configuring the API to accept the signing algorithm since it has already been configured in the [Startip](Startup.cs).cs file. Click on permissions to create the permissions for the API.
<h1 align "center">
    <img src="Images/Configuration/configure_api.png" width=100%>
</h1>

The permissions ensure that a validated user can interact with the API with a token that has not expired. Ensure that the permissions match, the description can change but the permission identifier must match.
<h1 align="center">
    <img src="Images/Configuration/permissions.png" width=100%>
</h1>

On the left side, click on Application and create a new Application. Choose the Machine to Machine Application
<h1 align="center">
    <img src="Images/Configuration/create_m2m.png" width=100%>
</h1>

With the grant permissions you created from the API, enable all the permissions. This is important because if they are not enabled then even with a valid token the request will return 403 (unauthorized)
<h1 align="center">
    <img src="Images/Configuration/authorize_app.png" width=100%>
</h1>

From the Application page, copy the client id and client secret. These values will be used for the API to interact with API.
<h1 align="center">
    <img src="Images/Configuration/api_cred.png" width=100%>
</h1>
Enter the information in the corresponding ``authcredentials.json`` file  
```Json
{
  "domain": "somedomain.auth0.com",
  "api_identifier": "https://squawk/api"
  "client_id": "clientidhere",
  "client_secret": "illkeepyoumydirtylittlesecret"
}
```

### API filesystem paths

For the purposes of properly uploading, downloading, updating, deleting, and streaming songs the API filesystem paths must be configured. For that purpose you have to open the ``paths.json`` file. What is meant by this is that the `root_music_path` directory where all music will be stored must exist. The `cover_root_path`, `archive_root_path`, and `temp_root_path` paths must exist and be accessible. An example on a Linux system:
```Json
{
  "root_music_path": "/dev/null/music/",
  "temp_root_path": "/dev/null/music/temp/",
  "cover_root_path": "/dev/null/music/coverArt/",
  "archive_root_path": "/dev/null/music/archive/"
}
```
* `root_music_path` - Where music will be stored in the following convention: *`Artist/Album/Songs`*
* `temp_music_path` - Where music will be stored when uploding songs to the server until the metadata has been fully parsed and entered into the database. Upon completion the files will be deleted and moved to the appropriate path in the `root_music_path`
* `cover_root_path` - Where cover art of music will be saved to.
* `archive_root_path` - When downloading compressed songs this is the path where songs will be compressed prior to dataa being read into memory, deleting the compressed file, and sending the compressed file from memory to the client


**Note**: The `temp_root_path`, `cover_root_path`, or `archive_root_path` does not have to be located in the same parent directory as `root_music_path`. Ensure that the permissions are properly set for all of the paths.

### Database connection string

In order for Database functionality to be operable, there must be a valid connection string and MySQL credentials with appropriate permissions. **At the moment there is only support for MySQL**. Edit the database.json file accordingly. An example of the fields to change are below:
```Json
{

  "server": "localhost", 
  "database": "my_db",
  "username": "admin",
  "password": "toughpassword"
 
}
```
* server - The address or domain name of the MySQL server
* database - The database name
* username - Username
* password - Self-explanatory

The only requirement of the User is that the user should have full permissions to the database as well as permissions to create a database. Other than that, that is all that is required.

### Database

Prior to starting the API, the database must be created. The following tables are required:
* User
* Salt
* Song
* Album
* Artist
* Year
* Genre
* CoverArt

There is a MySQL script to create these tables, it can be found in the [Scripts/MySQL/](https://github.com/amazing-username/Icarus/blob/master/Scripts/MySQL/create_database.sql) directory. Just merely execute:
```shell
mysql -u dblikedecibel -p < Scripts/MySQL/create_database.sql
```

From this point the database has been successfully created. Metadata and song filesystem locations can be saved.


## Building and Running
```
git clone --recursive https://github.com/kdeng00/icarus
cd 3rdparty/libbcrypt/
make
cp bcrypt.a libbcrypt.a
cp bcrypt.o libbcrypt.o
cd ../..
mkdir build
cd build
conan install ..
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DUSE_SYSTEM_CURL=ON -DBUILD_CPR_TESTS=OFF -DOATPP_BUILD_TESTS=OFF
make
bin/icarus
```
Runs the server on localhost port 5002

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on the code of conduct, and the process for submitting pull requests to the project.

## Authors

* **Kun Deng** - [amazing-username](https://github.com/amazing-username)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

