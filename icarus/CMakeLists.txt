cmake_minimum_required(VERSION 3.10)


if(NOT DEFINED VCPKG_TRIPLET)
    message("VCPKG_TRIPLET has not been defined...")
    set(VCPKG_TRIPLET "x64-linux")
    message("Using default ${VCPKG_TRIPLET} triplet")
endif()


message("toolchain file ${CMAKE_TOOLCHAIN_FILE}")

if(NOT DEFINED CMAKE_BUILD_TYPE)
    message("Building for debug")
    set(CMAKE_BUILD_TYPE DEBUG)

    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
    message("Exporting compile commands")
elseif(DEFINED CMAKE_BUILD_TYPE)

    if(NOT DEFINED ${CMAKE_BUILD_TYPE})
        message("CMAKE_BUILD_TYPE has not been defined. Defaulting to debug")
        set(CMAKE_BUILD_TYPE DEBUG)

	    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	    message("Exporting compile commands")
	elseif("${CMAKE_BUILD_TYPE}" STREQUAL "DEBUG")
	    message("Building for debug")
	
	    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
	    message("Exporting compile commands")
    elseif("${CMAKE_BUILD_TYPE}" STREQUAL "RELEASE")
        message("Building for release")
    endif()

    message("CMAKE_BUILD_TYPE is defined as ${CMAKE_BUILD_TYPE}")
endif()


set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "enforcing c++ standard requirement" FORCE)
set(CMAKE_CXX_STANDARD 17 CACHE STRING "setting c++ standard" FORCE)

message("Using c++ ${CMAKE_CXX_STANDARD} features")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

message("runtime output directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")


set(SOURCES
    src/callback/StreamCallback.cpp
    src/database/AlbumRepository.cpp
    src/database/ArtistRepository.cpp
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
    src/manager/YearManager.cpp
    src/utility/ImageFile.cpp
    src/utility/MetadataRetriever.cpp
    src/utility/PasswordEncryption.cpp
)


set(ICARUS_INCLUDE_DIR
    "${CMAKE_CURRENT_LIST_DIR}/include"
)

set (BCRYPTLIB
    ${CMAKE_CURRENT_LIST_DIR}/3rdparty/libbcrypt
)


include_directories(${ICARUS_INCLUDE_DIR} ${BCRYPTLIB} ${CMAKE_CURRENT_LIST_DIR}/3rdparty/vcpkg/installed/${VCPKG_TRIPLET}/include/mysql)
include_directories(${CMAKE_BINARY_DIR}/icarus/3rdparty/soci/include)


add_library(bcrypt STATIC IMPORTED)

set_target_properties(bcrypt PROPERTIES IMPORTED_LOCATION
    "${CMAKE_CURRENT_LIST_DIR}/3rdparty/libbcrypt/bcrypt.a"
)

configure_file(${CMAKE_CURRENT_LIST_DIR}/authcredentials.json ${CMAKE_BINARY_DIR}/bin/authcredentials.json COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/database.json ${CMAKE_BINARY_DIR}/bin/database.json COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/icarus_keys.json ${CMAKE_BINARY_DIR}/bin/icarus_keys.json COPYONLY)
configure_file(${CMAKE_CURRENT_LIST_DIR}/paths.json ${CMAKE_BINARY_DIR}/bin/paths.json COPYONLY)
configure_file(${CMAKE_SOURCE_DIR}/Images/Stock/CoverArt.png ${CMAKE_BINARY_DIR}/bin/CoverArt.png COPYONLY)


set(OATPP_BUILD_TESTS OFF CACHE BOOL "enable oatpp tests" FORCE)
set(BUILD_CPR_TESTS OFF CACHE BOOL "enable cpr tests" FORCE)
# TODO: Confirm if this uses the curl lib from VCPKG
set(USE_SYSTEM_CURL OFF)

find_package(nlohmann_json CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(cpr CONFIG REQUIRED)
find_package(oatpp CONFIG REQUIRED)

find_path(JWT_CPP_INCLUDE_DIRS "jwt-cpp/base.h")



set(SOCI_TESTS OFF CACHE BOOL "enable soci tests" FORCE)
set(SOCI_EMPTY OFF CACHE BOOL "enable soci empty" FORCE)
set(WITH_MYSQL ON CACHE BOOL "enable mysql" FORCE)
set(WITH_ODBC OFF CACHE BOOL "enable ODBC" FORCE)
set(WITH_SQLITE3 OFF CACHE BOOL "enable sqlite3" FORCE)
set(WITH_BOOST OFF CACHE BOOL "enable boost" FORCE)
set(WITH_DB2 OFF CACHE BOOL "enable DB2" FORCE)
set(WITH_FIREBIRD OFF CACHE BOOL "enable firebird" FORCE)
set(WITH_ORACLE OFF CACHE BOOL "enable oracle" FORCE)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/3rdparty/soci)


add_executable(icarus ${SOURCES})


target_link_libraries(icarus PRIVATE "-lstdc++fs" oatpp::oatpp tag nlohmann_json nlohmann_json::nlohmann_json)
target_link_libraries(icarus PRIVATE OpenSSL::SSL OpenSSL::Crypto CURL::libcurl cpr bcrypt icarus_lib)
target_link_libraries(icarus PRIVATE soci_core soci_mysql)
target_include_directories(icarus PRIVATE ${JWT_CPP_INCLUDE_DIRS})


if(DEFINED CMAKE_EXPORT_COMPILE_COMMANDS)
	if("${CMAKE_EXPORT_COMPILE_COMMANDS}" STREQUAL "ON")
        message("Adding custom command")

        add_custom_command(TARGET icarus PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/compile_commands.json" "${CMAKE_SOURCE_DIR}/compile_commands.json"
	        COMMENT "Copied the compile_commands.json file to project root"
	    )
	endif()
endif()
