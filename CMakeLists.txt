cmake_minimum_required(VERSION 3.10)

project(icarus)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/callback/StreamCallback.cpp
    src/database/AlbumRepository.cpp
    src/database/ArtistRepository.cpp
    src/database/BaseRepository.cpp
    src/database/CoverArtRepository.cpp
    src/database/GenreRepository.cpp
    src/database/SongRepository.cpp
    src/database/UserRepository.cpp
    src/database/YearRepository.cpp
    src/dto/conversion/DtoConversions.cpp
    src/Main.cpp
    src/manager/AlbumManager.cpp
    src/manager/ArtistManager.cpp
    src/manager/CoverArtManager.cpp
    src/manager/DirectoryManager.cpp
    src/manager/GenreManager.cpp
    src/manager/SongManager.cpp
    src/manager/TokenManager.cpp
    src/manager/UserManager.cpp
    src/manager/YearManager.cpp
    src/utility/ImageFile.cpp
    src/utility/MetadataRetriever.cpp
    src/utility/PasswordEncryption.cpp
    src/verify/Initialization.cpp
)
set(HEADERS
    include/callback/StreamCallback.h
    include/component/AppComponent.hpp
    include/controller/AlbumController.hpp
    include/controller/ArtistController.hpp
    include/controller/CoverArtController.hpp
    include/controller/GenreController.hpp
    include/controller/LoginController.hpp
    include/controller/RegisterController.hpp
    include/controller/SongController.hpp
    include/controller/YearController.hpp
    include/database/AlbumRepository.h
    include/database/ArtistRepository.h
    include/database/BaseRepository.h
    include/database/CoverArtRepository.h
    include/database/GenreRepository.h
    include/database/SongRepository.h
    include/database/UserRepository.h
    include/database/YearRepository.h
    include/dto/AlbumDto.hpp
    include/dto/ArtistDto.hpp
    include/dto/CoverArtDto.hpp
    include/dto/GenreDto.hpp
    include/dto/LoginResultDto.hpp
    include/dto/SongDto.hpp
    include/dto/YearDto.hpp
    include/dto/conversion/DtoConversions.h
    include/manager/AlbumManager.h
    include/manager/ArtistManager.h
    include/manager/CoverArtManager.h
    include/manager/DirectoryManager.h
    include/manager/GenreManager.h
    include/manager/SongManager.h
    include/manager/TokenManager.h    
    include/manager/UserManager.h
    include/manager/YearManager.h
    include/model/Models.h
    include/utility/ImageFile.h
    include/utility/MetadataRetriever.h
    include/utility/PasswordEncryption.h
    include/type/AlbumFilter.h
    include/type/ArtistFilter.h
    include/type/CoverFilter.h
    include/type/GenreFilter.h
    include/type/PathType.h
    include/type/SaltFilter.h
    include/type/Scopes.h
    include/type/SongChanged.h
    include/type/SongFilter.h
    include/type/SongUpload.h
    include/type/UserFilter.h
    include/type/YearFilter.h
    include/verify/Initialization.h
)

set (BCRYPTLIB
    ${CMAKE_SOURCE_DIR}/3rdparty/libbcrypt
)


#find_library(BCRYPT bcrypt ${BCRYPTLIB})

include_directories(include ${BCRYPTLIB})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/oatpp)

add_library(bcrypt STATIC IMPORTED)

set_target_properties(bcrypt PROPERTIES IMPORTED_LOCATION
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libbcrypt/bcrypt.a"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_BINARY_DIR}/bin/appsettings.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/authcredentials.json ${CMAKE_BINARY_DIR}/bin/authcredentials.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/database.json ${CMAKE_BINARY_DIR}/bin/database.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/paths.json ${CMAKE_BINARY_DIR}/bin/paths.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Images/Stock/CoverArt.png ${CMAKE_BINARY_DIR}/bin/CoverArt.png COPYONLY)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)

add_executable(icarus ${SOURCES} ${HEADERS})


target_include_directories(icarus PRIVATE ${JWT_CPP_INCLUDE_DIRS})
target_link_libraries(icarus PRIVATE "-lstdc++fs" oatpp tag mariadb nlohmann_json nlohmann_json::nlohmann_json cpr bcrypt)
