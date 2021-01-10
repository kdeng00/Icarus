# Icarus

Icarus is a music streaming API Server, allowing access to stream your personal music collection. Currently Icarus only supports Linux. Specifically 64-bit linux systems.

### Interfacing With Icarus

One can interface with Icarus the music server either by:

* [Mear](https://github.com/amazing-username/mear) - Partially implemented (under development)
* [IcarusDownloadManager](https://github.com/amazing-username/IcarusDownloadManager)
* Create your own client to interact with the API



## Built With


* C++
* Compiler with C++17 support
* [json](https://www.github.com/nlohmann/json)
* [cpr](https://www.github.com/whoshuu/cpr)
* [libcurl](https://www.github.com/curl/curl)
* [libopenssl](https://www.github.com/openssl/openssl)
* libmariadb
* [soci](https://github.com/soci/soci)
* [TagLib](https://github.com/taglib/taglib)
* [oatpp](https://github.com/oatpp/oatpp)
* [jwt-cpp](https://github.com/Thalhammer/jwt-cpp)
* [libbcrypt](https://github.com/rg3/libbcrypt)

![image](https://user-images.githubusercontent.com/14333136/56252069-28532d00-6084-11e9-896d-1a3c378014ef.png)

## Getting started
There are several things that need to be completed to properly setup and secure the API.
1. Create public and private keys
2. API filesystem paths
3. Database connection string
4. Migrations


### Creating public and private keys
Make sure you have openssl installed and have it either in your path environment variable or execute the software via a path. Have somewhere that you would like to store the public and private keys on the filesystem. Once you have that in mind let's just call that the ``root_path`` ``/home/soko/icarus/keys/``. The ``private`` and ``public`` keys will be stored in that directory with the following names respectively ``icarus_private-key.pem`` and ``icarus_public-key.pem``. You must be comfortable enough with the command line to run a few commands.
1. Change directory to the ``root_path``
```Bash
cd root_path
```
2. Create the private key
```Bash
openssl genrsa -out private 2048
```
3. Create the public key
```Bash
openssl rsa -in private -pubout -out public
```
4. Take notice of the absolute paths of the keys and save the paths in the ``icarus_keys.json`` file like so:
```Json
    "rsa_public_path": "/home/soko/icarus/keys/icarus_public-key.pem",
    "rsa_private_path": "/home/soko/icarus/keys/icarus_private-key.pem"
```

That's it.


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

cd icarus/3rdparty/vcpkg
./bootstrap-vcpkg.sh
./vcpkg install nlohmann-json curl openssl cpr taglib jwt-cpp libmariadb oatpp

cd ../../..
make -C icarus/3rdparty/libbcrypt

cmake -B build -S .
cmake --build build -j
build/bin/icarus
```
Runs the server on localhost port 5002

## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on the code of conduct, and the process for submitting pull requests to the project.

## Authors

* **Kun Deng** - [amazing-username](https://github.com/amazing-username)

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details

