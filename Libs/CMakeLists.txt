cmake_minimum_required(VERSION 3.10)

project(icarus)

set(CMAKE_CXX_STANDARD 17)
set(SOURCES
    src/icarus.cpp
)

add_library(icarus SHARED ${SOURCES})
