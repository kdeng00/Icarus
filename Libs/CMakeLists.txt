cmake_minimum_required(VERSION 3.10)

project(icarus)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/directory_manager.cpp
    src/imageFile.cpp
    src/metadata_retriever.cpp
    src/token_manager.cpp
)
set(HEADERS
    include/models.h
    include/directory_manager.h
    include/imageFile.h
    include/metadata_retriever.h
)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(cpr)

add_library(icarus SHARED ${SOURCES} ${HEADERS})
include_directories(${CPR_INCLUDE_DIRS} include)
target_link_libraries(icarus "-lstdc++fs" tag ${CONAN_LIBS} ${CPR_LIBRARIES})
