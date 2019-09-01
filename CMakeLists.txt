cmake_minimum_required(VERSION 3.10)

project(icarus)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/appComponent.hpp
    src/database/albumRepository.cpp
    src/database/artistRepository.cpp
    src/database/base_repository.cpp
    src/database/coverArtRepository.cpp
    src/database/genreRepository.cpp
    src/database/songRepository.cpp
    src/database/yearRepository.cpp
    src/main.cpp
    src/managers/albumManager.cpp
    src/managers/artistManager.cpp
    src/managers/coverArtManager.cpp
    src/managers/directory_manager.cpp
    src/managers/genreManager.cpp
    src/managers/song_manager.cpp
    src/managers/token_manager.cpp
    src/managers/yearManager.cpp
    src/utilities/imageFile.cpp
    src/utilities/metadata_retriever.cpp
)
set(HEADERS
    include/controller/loginController.hpp
    include/controller/songController.hpp
    include/database/albumRepository.h
    include/database/artistRepository.h
    include/database/base_repository.h
    include/database/coverArtRepository.h
    include/database/genreRepository.h
    include/database/songRepository.h
    include/database/yearRepository.h
    include/dto/loginResultDto.hpp
    include/dto/songDto.hpp
    include/managers/albumManager.h
    include/managers/artistManager.h
    include/managers/coverArtManager.h
    include/managers/directory_manager.h
    include/managers/genreManager.h
    include/managers/song_manager.h
    include/managers/token_manager.h
    include/managers/yearManager.h
    include/models/models.h
    include/utilities/imageFile.h
    include/utilities/metadata_retriever.h
    include/types/coverFilter.h
    include/types/scopes.h
    include/types/songFilter.h
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
