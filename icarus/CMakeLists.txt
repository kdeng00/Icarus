cmake_minimum_required(VERSION 3.10)


set(VCPKG_ROOT_PATH ${CMAKE_SOURCE_DIR}/3rdparty/vcpkg)

message("The vcpkg root path ${VCPKG_ROOT_PATH}")

set(TOOLCHAIN_PATH ${VCPKG_ROOT_PATH}/scripts/buildsystems/vcpkg.cmake)

set(CMAKE_TOOLCHAIN_FILE
    ${TOOLCHAIN_PATH}
    CACHE STRING "Vcpkg toolchain file")

message("toolchain file ${CMAKE_TOOLCHAIN_FILE}")



set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message("runtime output directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


set(OATPP_BUILD_TESTS OFF)
set(BUILD_CPR_TESTS OFF)
set(USE_SYSTEM_CURL OFF)

set(SOURCES
    src/callback/StreamCallback.cpp
    src/database/AlbumRepository.cpp
    src/database/ArtistRepository.cpp
    src/database/BaseRepository.cpp
    src/database/CoverArtRepository.cpp
    src/database/GenreRepository.cpp
    src/database/SongRepository.cpp
    src/database/YearRepository.cpp
    src/Main.cpp
    src/manager/AlbumManager.cpp
    src/manager/ArtistManager.cpp
    src/manager/CoverArtManager.cpp
    src/manager/DirectoryManager.cpp
    src/manager/GenreManager.cpp
    src/manager/SongManager.cpp
    src/manager/TokenManager.cpp
    src/manager/YearManager.cpp
    src/utility/ImageFile.cpp
    src/utility/MetadataRetriever.cpp
    src/utility/PasswordEncryption.cpp
)


set(ICARUS_INCLUDE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

set (BCRYPTLIB
    ${CMAKE_CURRENT_LIST_DIR}/../3rdparty/libbcrypt
)


include_directories(${ICARUS_INCLUDE_DIR} ${BCRYPTLIB})


add_library(bcrypt STATIC IMPORTED)

set_target_properties(bcrypt PROPERTIES IMPORTED_LOCATION
    "${CMAKE_CURRENT_LIST_DIR}/../3rdparty/libbcrypt/bcrypt.a"
)

configure_file(${CMAKE_CURRENT_LIST_DIR}/authcredentials.json ${CMAKE_BINARY_DIR}/bin/authcredentials.json COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/database.json ${CMAKE_BINARY_DIR}/bin/database.json COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/paths.json ${CMAKE_BINARY_DIR}/bin/paths.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Images/Stock/CoverArt.png ${CMAKE_BINARY_DIR}/bin/CoverArt.png COPYONLY)


find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(oatpp CONFIG REQUIRED)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")



add_executable(icarus ${SOURCES})


target_link_libraries(icarus PRIVATE "-lstdc++fs" oatpp::oatpp tag mariadb nlohmann_json nlohmann_json::nlohmann_json OpenSSL::SSL OpenSSL::Crypto CURL::libcurl cpr bcrypt icarus_lib)
target_include_directories(icarus PRIVATE ${JWT_CPP_INCLUDE_DIRS})
