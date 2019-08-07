cmake_minimum_required(VERSION 3.10)

project(icarus CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/directory_manager.cpp
    src/metadata_retriever.cpp
)
set(HEADERS
    include/models.h
    include/directory_manager.h
    include/metadata_retriever.h
)

add_library(icarus SHARED ${SOURCES} ${HEADERS})
include_directories(include)
target_link_libraries(icarus "-lstdc++fs" tag)
