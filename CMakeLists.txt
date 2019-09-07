cmake_minimum_required(VERSION 3.10)

project(icarus)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
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
)
set(HEADERS
    include/component/AppComponent.hpp
    include/controller/AlbumController.hpp
    include/controller/ArtistController.hpp
    include/controller/CoverArtController.hpp
    include/controller/GenreController.hpp
    include/controller/LoginController.hpp
    include/controller/SongController.hpp
    include/controller/YearController.hpp
    include/database/AlbumRepository.h
    include/database/ArtistRepository.h
    include/database/BaseRepository.h
    include/database/CoverArtRepository.h
    include/database/GenreRepository.h
    include/database/SongRepository.h
    include/database/YearRepository.h
    include/dto/AlbumDto.hpp
    include/dto/ArtistDto.hpp
    include/dto/CoverArtDto.hpp
    include/dto/GenreDto.hpp
    include/dto/LoginResultDto.hpp
    include/dto/SongDto.hpp
    include/dto/YearDto.hpp
    include/manager/AlbumManager.h
    include/manager/ArtistManager.h
    include/manager/CoverArtManager.h
    include/manager/DirectoryManager.h
    include/manager/GenreManager.h
    include/manager/SongManager.h
    include/manager/TokenManager.h
    include/manager/YearManager.h
    include/model/Models.h
    include/utility/ImageFile.h
    include/utility/MetadataRetriever.h
    include/type/AlbumFilter.h
    include/type/ArtistFilter.h
    include/type/CoverFilter.h
    include/type/GenreFilter.h
    include/type/Scopes.h
    include/type/SongFilter.h
    include/type/YearFilter.h
)

set (TAGLIB
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/taglib/3rdparty
)

set(TAGLIB_HEADERS
    "${CMAKE_SOURCE_DIR}/build/3rdparty/taglib" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/ape" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/asf" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/dsdiff" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/dsf" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/flac" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/it" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/mod" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/mp4" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/mpc" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/mpeg" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/mpeg/id3v2" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/mpeg/id3v2/frames" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/ogg" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/riff" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/s3m" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/toolkit" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/trueaudio" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/wavpack" 
    "${CMAKE_SOURCE_DIR}/3rdparty/taglib/taglib/xm" 
)

set(JWT_CPP_INCLUDE
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/jwt-cpp/include
)

set (ORM_DIR
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ormpp
)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include_directories(include ${CPR_INCLUDE_DIRS} ${TAGLIB} ${TAGLIB_HEADERS})

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/cpr)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/taglib)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/oatpp)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/appsettings.json ${CMAKE_BINARY_DIR}/bin/appsettings.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/authcredentials.json ${CMAKE_BINARY_DIR}/bin/authcredentials.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/database.json ${CMAKE_BINARY_DIR}/bin/database.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/paths.json ${CMAKE_BINARY_DIR}/bin/paths.json COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Images/Stock/CoverArt.png ${CMAKE_BINARY_DIR}/bin/CoverArt.png COPYONLY)

add_executable(icarus ${SOURCES} ${HEADERS})
target_include_directories(icarus PUBLIC ${JWT_CPP_INCLUDE})
target_link_libraries(icarus "-lstdc++fs" tag oatpp mysqlclient ${CONAN_LIBS} ${CPR_LIBRARIES})
