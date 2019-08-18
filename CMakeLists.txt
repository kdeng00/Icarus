cmake_minimum_required(VERSION 3.10)

project(icarus)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/appComponent.hpp
    src/controller/loginController.hpp
    src/database/base_repository.cpp
    src/directory_manager.cpp
    src/dto/loginResultDto.hpp
    src/imageFile.cpp
    src/main.cpp
    src/metadata_retriever.cpp
    src/token_manager.cpp
)
set(HEADERS
    include/database/base_repository.h
    include/directory_manager.h
    include/imageFile.h
    include/metadata_retriever.h
    include/models.h
    include/token_manager.h
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

add_executable(icarus ${SOURCES} ${HEADERS})
target_include_directories(icarus PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/jwt-cpp/include)
target_link_libraries(icarus "-lstdc++fs" tag oatpp mysqlclient ${CONAN_LIBS} ${CPR_LIBRARIES})
