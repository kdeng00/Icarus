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


set(ICARUS_INCLUDE_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set (BCRYPTLIB
    ${CMAKE_SOURCE_DIR}/3rdparty/libbcrypt
)


#find_library(BCRYPT bcrypt ${BCRYPTLIB})

include_directories(${ICARUS_INCLUDE_DIR} ${BCRYPTLIB})

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/oatpp)

add_library(bcrypt STATIC IMPORTED)

set_target_properties(bcrypt PROPERTIES IMPORTED_LOCATION
        "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libbcrypt/bcrypt.a"
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_BINARY_DIR}/bin/appsettings.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/authcredentials.json ${CMAKE_BINARY_DIR}/bin/authcredentials.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/database.json ${CMAKE_BINARY_DIR}/bin/database.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/paths.json ${CMAKE_BINARY_DIR}/bin/paths.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Images/Stock/CoverArt.png ${CMAKE_BINARY_DIR}/bin/CoverArt.png COPYONLY)
# configure_file(${CMAKE_CURRENT_SOURCE_DIR}/build/icarus ${CMAKE_BINARY_DIR}/bin/icarus COPYONLY)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")

find_package(nlohmann_json CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(oatpp CONFIG REQUIRED)

add_executable(icarus ${SOURCES})




target_include_directories(icarus PRIVATE ${JWT_CPP_INCLUDE_DIRS})
target_link_libraries(icarus PRIVATE "-lstdc++fs" oatpp::oatpp tag mariadb nlohmann_json nlohmann_json::nlohmann_json cpr bcrypt)
